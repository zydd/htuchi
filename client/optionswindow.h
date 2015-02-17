#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include <QDialog>
#include <QSettings>

namespace Ui
{
class OptionsWindow;
}

class OptionsWindow : public QDialog
{
    Q_OBJECT
public:
    OptionsWindow(QWidget *parent = nullptr);
    virtual void accept();

private:
    Ui::OptionsWindow *ui;
    QSettings _settings;

signals:
    void infoUpdated();
};

#endif // OPTIONSWINDOW_H
