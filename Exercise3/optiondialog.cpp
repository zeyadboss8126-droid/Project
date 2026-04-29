#include "optiondialog.h"
#include "ui_optiondialog.h"

#include "ModelPart.h"
#include <QColor>

OptionDialog::OptionDialog(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::OptionDialog)
{
    ui->setupUi(this);
}

OptionDialog::~OptionDialog()
{
    delete ui;
}

void OptionDialog::setModelPart(ModelPart* part)
{
    m_part = part;
    if (!m_part) return;

    ui->lineEditName->setText(m_part->data(0).toString());

    QString visText = m_part->data(1).toString().trimmed().toLower();
    bool visible = (visText == "true" || visText == "1" || visText == "yes");
    ui->checkVisible->setChecked(visible);

    QString colourText = m_part->data(2).toString().trimmed();
    int r = 255, g = 255, b = 255;

    const QStringList parts = colourText.split(',', Qt::SkipEmptyParts);

    if (parts.size() == 3) {
        r = parts[0].trimmed().toInt();
        g = parts[1].trimmed().toInt();
        b = parts[2].trimmed().toInt();
    }

    ui->spinR->setValue(r);
    ui->spinG->setValue(g);
    ui->spinB->setValue(b);

    ui->checkShrink->setChecked(m_part->shrinkFilterEnabled());
    ui->checkClip->setChecked(m_part->clipFilterEnabled());
}

void OptionDialog::accept()
{
    if (m_part)
    {
        m_part->setName(ui->lineEditName->text());

        m_part->setVisible(ui->checkVisible->isChecked());

        m_part->setColour(
            ui->spinR->value(),
            ui->spinG->value(),
            ui->spinB->value()
        );

        m_part->setShrinkFilter(ui->checkShrink->isChecked());
        m_part->setClipFilter(ui->checkClip->isChecked());
    }

    QDialog::accept();
}