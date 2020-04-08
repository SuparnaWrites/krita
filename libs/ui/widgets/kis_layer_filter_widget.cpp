#include "kis_layer_filter_widget.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QCompleter>
#include <QEvent>
#include <QMouseEvent>
#include <QButtonGroup>

#include "kis_debug.h"
#include "kis_node.h"
#include "kis_color_label_button.h"
#include "kis_color_label_selector_widget.h"
#include "kis_node_view_color_scheme.h"

KisLayerFilterWidget::KisLayerFilterWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    textFilter = new QLineEdit(this);
    textFilter->setPlaceholderText("Search...");
    textFilter->setMinimumWidth(256);
    textFilter->setMinimumHeight(32);
    textFilter->setClearButtonEnabled(true);


    connect(textFilter, SIGNAL(textChanged(QString)), this, SIGNAL(filteringOptionsChanged()));

    KisNodeViewColorScheme colorScheme;

    QWidget *buttonContainer = new QWidget(this);
    buttonEventFilter = new EventFilter(buttonContainer);
    {
        QHBoxLayout *subLayout = new QHBoxLayout(buttonContainer);
        buttonContainer->setLayout(subLayout);
        subLayout->setContentsMargins(0,0,0,0);
        QButtonGroup *btnGroup = new QButtonGroup(buttonContainer);
        btnGroup->setExclusive(false);
        foreach (const QColor &color, colorScheme.allColorLabels()) {
            KisColorLabelButton* btn = new KisColorLabelButton(color, buttonContainer);
            btnGroup->addButton(btn);
            btn->installEventFilter(buttonEventFilter);
            btn->setVisible(false);
            subLayout->addWidget(btn);
            colorLabelButtons.append(btn);
        }

        connect(btnGroup, SIGNAL(buttonToggled(int,bool)), this, SIGNAL(filteringOptionsChanged()) );
    }

    layout->addWidget(textFilter);
    layout->addWidget(buttonContainer);
}

void KisLayerFilterWidget::scanUsedColorLabels(KisNodeSP node, QSet<int> &colorLabels) {

    if (node->parent()) {
        colorLabels.insert(node->colorLabelIndex());
    }

    KisNodeSP child = node->firstChild();
    while(child) {
        scanUsedColorLabels(child, colorLabels);
        child = child->nextSibling();
    }
}

void KisLayerFilterWidget::updateColorLabels(KisNodeSP root) {
    QSet<int> colorLabels;

    scanUsedColorLabels(root, colorLabels);

    if (colorLabels.size() > 1) {
        colorLabelButtons[0]->parentWidget()->setVisible(true);

        for (size_t index = 0; index < colorLabelButtons.size(); index++) {
            if (colorLabels.contains(index)) {
                colorLabelButtons[index]->setVisible(true);
            } else {
                colorLabelButtons[index]->setVisible(false);
                colorLabelButtons[index]->setChecked(true);
            }
        }
    } else {
        colorLabelButtons[0]->parentWidget()->setVisible(false);
    }

    filteringOptionsChanged();
}

bool KisLayerFilterWidget::isCurrentlyFiltering()
{
    const bool isFilteringText = !textFilter->text().isEmpty();

    bool isFilteringColors = false;
    for (int index = 0; index < colorLabelButtons.size(); index++) {
        if (colorLabelButtons[index]->isVisible() && !colorLabelButtons[index]->isChecked()) {
            isFilteringColors = true;
        }
    }

    return isFilteringText || isFilteringColors;
}

QList<int> KisLayerFilterWidget::getActiveColors()
{
    QList<int> activeColors;

    for (int index = 0; index < colorLabelButtons.size(); index++) {
        if (colorLabelButtons[index]->isVisible() && colorLabelButtons[index]->isChecked()) {
            activeColors.append(index);
        }
    }

    return activeColors;
}

QString KisLayerFilterWidget::getTextFilter()
{
    return textFilter->text();
}

void KisLayerFilterWidget::reset()
{
    textFilter->clear();

    for (int index = 0; index < colorLabelButtons.size(); index++) {
        colorLabelButtons[index]->setChecked(true);
    }
    filteringOptionsChanged();
}

KisLayerFilterWidget::EventFilter::EventFilter(QWidget *buttonContainer, QObject* parent) : QObject(parent) {
    m_buttonContainer = buttonContainer;
    lastKnownMousePosition = QPoint(0,0);
}

bool KisLayerFilterWidget::EventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        currentState = WaitingForDragLeave;
        lastKnownMousePosition = mouseEvent->globalPos();

        return true;

    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        //If we never left, toggle the original button.
        if( currentState == WaitingForDragLeave ) {
            QAbstractButton* btn = static_cast<QAbstractButton*>(obj);
            tryToggleButton(btn);
        }

        currentState = Idle;
        lastKnownMousePosition = mouseEvent->globalPos();

        return true;

    } else if (event->type() == QEvent::MouseMove ) {

        if (currentState == WaitingForDragLeave) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            QWidget* firstClicked = static_cast<QWidget*>(obj);
            const QPointF localPosition = mouseEvent->localPos();

            if (!firstClicked->rect().contains(localPosition.x(), localPosition.y()))
            {
                QAbstractButton* btn = static_cast<QAbstractButton*>(obj);
                tryToggleButton(btn);

                checkSlideOverNeighborButtons(mouseEvent, btn);

                currentState = WaitingForDragEnter;
            }

            lastKnownMousePosition = mouseEvent->globalPos();

            return true;

        } else if (currentState == WaitingForDragEnter) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            QAbstractButton* startingButton = static_cast<QAbstractButton*>(obj);
            const QPoint currentPosition = mouseEvent->globalPos();

            checkSlideOverNeighborButtons(mouseEvent, startingButton);

            lastKnownMousePosition = currentPosition;

            return true;
        }

    }

    return false;
}

void KisLayerFilterWidget::EventFilter::checkSlideOverNeighborButtons(QMouseEvent* mouseEvent, QAbstractButton* startingButton)
{
    const QPoint currentPosition = mouseEvent->globalPos();

    if (startingButton->group()) {
        QList<QAbstractButton*> allButtons = startingButton->group()->buttons();

        Q_FOREACH(QAbstractButton* button, allButtons) {
            const QRect bounds = QRect(button->mapToGlobal(QPoint(0,0)), button->size());
            const QRect mouseMovement = QRect(lastKnownMousePosition, currentPosition);
            if( bounds.intersects(mouseMovement) && !bounds.contains(lastKnownMousePosition)) {
                tryToggleButton(button);
            }
        }
    }
}

bool KisLayerFilterWidget::EventFilter::tryToggleButton(QAbstractButton *btn) const
{
    btn->toggle();

    if (btn->group()) {
        //todo QButtonGroup Extension for counting number of checked buttons?
        QButtonGroup* group = btn->group();

        if( !group->checkedButton() ) {
            btn->toggle();
            return false;
        } else {
            btn->toggled(btn->isChecked());
        }

    } else {
        btn->toggled(btn->isChecked());
    }

    return true;
}
