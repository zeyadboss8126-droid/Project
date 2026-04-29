// optiondialog.h
#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>

namespace Ui {
class OptionDialog;
}

class ModelPart;

class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionDialog(QWidget *parent = nullptr);
    ~OptionDialog();

    // Call this before exec() to edit a specific tree item
    void setModelPart(ModelPart* part);

protected:
    // Called when user presses OK (via button box accepted())
    void accept() override;

private:
    Ui::OptionDialog *ui;
    ModelPart* m_part = nullptr;   // MUST be a member (not global)
};

#endif // OPTIONDIALOG_H
