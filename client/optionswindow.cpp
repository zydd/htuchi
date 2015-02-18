#include <QInputDialog>
#include <QByteArray>
#include <sodium.h>

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
    if (_settings.value("key").isNull())
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
                QByteArray key(crypto_secretbox_KEYBYTES, Qt::Uninitialized);

//                 QByteArray salt(crypto_pwhash_scryptsalsa208sha256_SALTBYTES, Qt::Uninitialized);
//                 randombytes_buf(salt.data(), salt.size());
                QByteArray salt("0$DkP.qAf7Vjt7ULEs8y.begKxWeV4h9MpxI/kUbuI49Jy3VRhgX2HZ8xHwL7QSR",
                                crypto_pwhash_scryptsalsa208sha256_SALTBYTES);

                if (crypto_pwhash_scryptsalsa208sha256(
                        (unsigned char *) key.data(), key.size(), pass.constData(), pass.size(), (unsigned char *) salt.constData(),
                        crypto_pwhash_scryptsalsa208sha256_OPSLIMIT_INTERACTIVE,
                        crypto_pwhash_scryptsalsa208sha256_MEMLIMIT_INTERACTIVE) != 0) {
                    throw std::runtime_error("crypto_pwhash_scryptsalsa208sha256() failed");
                }

                _settings.setValue("key", key);
                _settings.setValue("key_nonce", salt);
            } else {
                return;  // Do not accept
            }
        }
    }
    QDialog::accept();
}

