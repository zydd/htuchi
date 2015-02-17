#include <QInputDialog>
#include <QByteArray>
#include <sodium.h>
#include <QDebug>

#include "optionswindow.h"
#include "ui_optionswindow.h"
#include "packet_util.h"

OptionsWindow::OptionsWindow(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::OptionsWindow),
      _settings("zydd", "htuchi")
{
    ui->setupUi(this);
    ui->nameEdit->setText(_settings.value("username").toString());
    if (_settings.value("hashed_password").isNull())
        ui->passwordEdit->setPlaceholderText(QString());

}

void OptionsWindow::accept()
{
    if (ui->nameEdit->text() != _settings.value("username").toString()) {
        _settings.setValue("username", ui->nameEdit->text());
        emit infoUpdated();
    }

    if (!ui->passwordEdit->text().isEmpty()) {
        bool ok;
        QString password = QInputDialog::getText(this, "Password", "Confirm password:", QLineEdit::Password, QString(), &ok);
        if (ok) {
            if (password == ui->passwordEdit->text()) {
                QByteArray pass = password.toUtf8();
                QByteArray hashed_password(crypto_pwhash_scryptsalsa208sha256_STRBYTES, Qt::Uninitialized);

                if (crypto_pwhash_scryptsalsa208sha256_str(
                        hashed_password.data(), pass.constData(), pass.size(),
                        crypto_pwhash_scryptsalsa208sha256_OPSLIMIT_SENSITIVE,
                        crypto_pwhash_scryptsalsa208sha256_MEMLIMIT_SENSITIVE) != 0) {
                    throw std::runtime_error("crypto_pwhash_scryptsalsa208sha256_str() failed");
                }

                _settings.setValue("hashed_password", hashed_password);
            } else {
                return;  // Do not accept
            }
        }
    }
    QDialog::accept();
}

