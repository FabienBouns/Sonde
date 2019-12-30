#include "transmission.h"

/**
* @file transmission.cpp
*
* @brief programme qui s'occupe de la parti trame (configuration, reception / envoie de trame)
*
* @author Bounoir Fabien
* @author Villesseche Ethan
*
* @version 2.1
*
*/

/**
 * @brief constructeur de la classe Transmission
 *
 * @fn Transmission::Transmission
 * @param parent
 */
Transmission::Transmission(QObject *parent) : QObject(parent), trame("")
{
    esp32 = new Esp32(this);
    port = new QSerialPort(this);
}

/**
 * @brief destructeur de la classe Transmission
 *
 * @fn Transmission::~Transmission
 */
Transmission::~Transmission()
{
    this->fermerPort();
}

/**
 * @brief return la trame stocker
 *
 * @fn Transmission::getTrame
 * @return QString
 */
QString Transmission::getTrame() const
{
    return trame;
}

/**
 * @brief return l'objet Esp32
 *
 * @fn Transmission::getEsp32
 * @return Esp32
 */
Esp32* Transmission::getEsp32() const
{
    return esp32;
}

/**
 * @brief configure le port serie
 *
 * @fn Transmission::configurerPort
 */
void Transmission::ouvrirPort()
{
    port->open(QIODevice::ReadWrite);
    if(port->isOpen())
    {
        qDebug() << __FUNCTION__ << ": " <<"Le port est ouvert";

        connect(port, SIGNAL(readyRead()), this, SLOT(recevoir()));
        emit portOuvert();
    }
    else
    {
        qDebug() << __FUNCTION__ << ": " << "Erreur, le port n'a pas pu être ouvert";
    }
}

/**
 * @brief configurer le port de communication
 *
 * @fn Transmission::configurerPort
 */
void Transmission::configurerPort(QString portCommunication, QString DebitBaud, QString BitsDonnees, QString BitsStop)
{
    port = new QSerialPort(portCommunication);

    port->setBaudRate(DebitBaud.toInt());
    port->setDataBits(QSerialPort::DataBits(BitsDonnees.toInt()));
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::StopBits(BitsStop.toInt()));
    port->setFlowControl(QSerialPort::NoFlowControl);

    ouvrirPort();
}

/**
 * @brief cette methode ferme le port serie
 *
 * @fn Transmission::fermerPort
 */
void Transmission::fermerPort()
{
    if(port->isOpen())
    {
        port->close();

        qDebug() << __FUNCTION__ << ": " <<"port fermer" <<endl;

        emit portFerme();
    }
}

/**
 * @brief   Décompose la trame reçue
 *
 * @fn Transmission::decomposer
 */
void Transmission::decomposer()
{
    if(trame != "OK\n")
    {
        esp32->setTemperature(trame.section(";",1,1).toDouble());
        esp32->setTemperatureUnite(trame.section(";",2,2));
        esp32->setRessentie(trame.section(";",3,3).toDouble());
        esp32->setRessentieUnite(trame.section(";",4,4));
        esp32->setHumidite(trame.section(";",5,5).toInt());
        esp32->setHumiditeUnite(trame.section(";",6,6));
        esp32->setLuminosite(trame.section(";",7,7).toInt());
        esp32->setLuminositeUnite(trame.section(";",8,8));
        esp32->setPression(trame.section(";",9,9).toInt());
        esp32->setPressionUnite(trame.section(";",10,10));
        esp32->setAltitude(trame.section(";",11,11).toInt());
        esp32->setAltitudeUnite(trame.section(";",12,12));
        esp32->setCouleurLed(trame.section(";",17,17).toInt());
    }
    emit trameRecue();
}

/**
 * @brief Récupérer la trame émise
 *
 * @fn Transmission::recevoir
 */
void Transmission::recevoir()
{
    QByteArray donnees;

        while(port->waitForReadyRead(10))
        {
            donnees += port->readAll();
        }
        trame = QString(donnees.data());

        if(trame != "")
        {
            qDebug() << __FUNCTION__ << ": " << trame << endl;

            this->decomposer();
        }
}

/**
 * @brief envoyer les données par le port serie
 *
 * @fn Transmission::envoyerDonnees
 * @param envoyerTrame
 */
void Transmission::envoyerDonnees(QString envoyerTrame)
{
    if(port->isOpen())
    {
        const char* trame = envoyerTrame.toStdString().c_str();
        port->write(trame);

        qDebug() << __FUNCTION__ << ": " << trame << endl;
    }
    else
    {
        qDebug() << __FUNCTION__ << " port non ouvert" << endl;
        emit portFerme();
    }
}