 #include "FileTab.h"
#include <QFile>
#include <QFile>
#include <QMessageBox>
#include <QJsonDocument>
#include "JSONPropertyWidget.h"

FileTabWidget::FileTabWidget(QString filePath, QWidget *parent)
	: QWidget(parent),FilePath(filePath)
{
	ui.setupUi(this);

    QWidget* result = new QWidget(this);

    scrollWidget = new QWidget(this);
    VerticalLayout = new QVBoxLayout(scrollWidget);
    ui.scrollArea->setWidget(scrollWidget);

    if (filePath != "new file")
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, "Warinig!", "Failed to open file!", QMessageBox::Ok, QMessageBox::Ok);
        }

        QString val;
        val = file.readAll();
        file.close();

        //load document and create JSON object to work with
        QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject docObject = document.object();

        QStringList keys = docObject.keys();

        fileObject = new JSONObjectWidget(docObject, this, "file", false);
        VerticalLayout->addWidget(fileObject);
    }
    else
    {
        fileObject = new JSONObjectWidget(QJsonObject(), this, "file", false);
        VerticalLayout->addWidget(fileObject);

        Edited = true;
    }

    connect(fileObject, &JSONObjectWidget::OnChangeInChild, this, &FileTabWidget::OnChangeInFile);
}

void FileTabWidget::OnChangeInFile(EditorActions::SEditorAction* action)
{
    if (action->ActionType != EditorActions::SEditorAction::Type::None)
    {
        Edited = true;
        Actions.append(action);
    }

    if (action->ActionType == EditorActions::SEditorAction::Type::TypeChange)
    {
        EditorActions::SPropertyTypeSelectionChangeAction*typeAction = static_cast<EditorActions::SPropertyTypeSelectionChangeAction*>(action);
        qWarning() << typeAction->OldValue;
      
    }

    if (action->ActionType == EditorActionType::IdInParentChange)
    {
        EditorActions::SWidgetIdChangeAction* Action = static_cast<EditorActions::SWidgetIdChangeAction*>(action);
        qWarning() << Action->MovedChild->Name;
    }
}

FileTabWidget::~FileTabWidget()
{
}

void FileTabWidget::UndoAction()
{
    /*based on the action itself different actions will happen
    There are limited amount of those
    1) Type change <- Done
    2) Name Change
    3) Value change <- Fields themselves have undo/redo feature
    4) Tree removal
    5) Tree addition
    6) Changing of id in parent
    */
    if (!Actions.empty())
    {
        auto act = Actions.at(Actions.count() - 1);
        if (act ->ActionType == EditorActionType::TypeChange)
        {
            EditorActions::SPropertyTypeSelectionChangeAction* action = static_cast<EditorActions::SPropertyTypeSelectionChangeAction*>(Actions.at(Actions.count() - 1));
            //we have to call same action but with reversed values

            //the event MUST use only JSONProperty so we don't do any checks
            JSONPropertyWidget* prop = static_cast<JSONPropertyWidget*>(action->ActionSoure);
            prop->ChangeTypeSelection(action->OldValue);

            //remove last action(because we already redid it)
            Actions.pop_back();
        }
        if(act->ActionType == EditorActionType::IdInParentChange)
        {
            EditorActions::SWidgetIdChangeAction* action = static_cast<EditorActions::SWidgetIdChangeAction*>(act);
            action->ActionSoure->ChangeChildId_NoSignal(action->OldId, action->MovedChild);

            //remove last action(because we already redid it)
            Actions.pop_back();
        }
        if (act->ActionType == EditorActionType::ValueChange)
        {
            //remove last action(because we already redid it)
            Actions.pop_back();
        }
    }
}
