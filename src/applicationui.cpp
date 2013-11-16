#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/Container>
#include <bb/cascades/Button>
#include <bb/data/JsonDataAccess>
#include <QtNetwork/QNetworkReply>
#include <memory>
#include <QUrl>
#include <core/SFAuthenticationManager.h>
#include <oauth/SFOAuthCredentials.h>
#include "RESTExplorerPageController.h"

QString const sf::SFRemoteAccessConsumerKey = "3MVG9y6x0357HleejRitlDvJj0_GfIS.kLS.OK0LCO6ifB8NxYkfM_0ZcT.J_wY25w4AJu2RIOEvv4INMaamm";
QString const sf::SFOAuthRedirectURI = "testsfdc:///mobilesdk/detect/oauth/done";
QString const sf::SFDefaultRestApiVersion = "/v28.0";

using namespace bb::cascades;
using namespace sf;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) : sf::SFAbstractApplicationUI(app)
{
	//for debugging purpose only
	qDebug()<<"organization name "<< QCoreApplication::organizationName();
	qDebug()<<"application name "<< QCoreApplication::applicationName();

	//required setup

    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);
    if(!QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()))) {
        // This is an abnormal situation! Something went wrong!
        // Add own code to recover here
        qWarning() << "Recovering from a failed connect()";
    }
    // initial load
    onSystemLanguageChanged();

    //register page controllers
    qmlRegisterType<RESTExplorerPageController>("sf.rest", 1, 0, "RESTExplorerPageController");


    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    //connect slots for authentication
	SFAuthenticationManager* authManager = SFAuthenticationManager::instance();

	connect(authManager, SIGNAL(SFOAuthFlowSuccess(SFOAuthInfo*)), this, SLOT(onSFOAuthFlowSuccess(SFOAuthInfo*)), Qt::UniqueConnection);
	connect(authManager, SIGNAL(SFOAuthFlowCanceled(SFOAuthInfo*)), this, SLOT(onSFOAuthFlowCanceled(SFOAuthInfo*)), Qt::UniqueConnection);
	connect(authManager, SIGNAL(SFLoginHostChanged()), this, SLOT(onSFLoginHostChanged()), Qt::UniqueConnection);
	connect(authManager, SIGNAL(SFOAuthFlowSuccess(SFOAuthInfo*)), this, SLOT(onSFOAuthFlowSuccess(SFOAuthInfo*)), Qt::UniqueConnection);
	connect(authManager, SIGNAL(SFOAuthFlowFailure(SFOAuthInfo*)), this, SLOT(onSFOAuthFlowFailure(SFOAuthInfo*)), Qt::UniqueConnection);
	connect(authManager, SIGNAL(SFUserLoggedOut()), this, SLOT(onSFUserLoggedOut()), Qt::UniqueConnection);

    // Set created root object as the application scene
    app->setScene(root);
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("SalesForceOAuth_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

/************************
 * UI Slots
 ************************/
void ApplicationUI::onSFOAuthFlowSuccess(SFOAuthInfo* info){
	qDebug()<<"[DEBUG] Authentication succeeded";
	qDebug()<<"[DEBUG] The authentication flow:"<<info->getAuthType();

	qDebug()<<"[DEBUG] Access token:";
	qDebug()<<SFAuthenticationManager::instance()->getCredentials()->getAccessToken();

	qDebug()<<"[DEBUG] Instance URL:";
	qDebug()<<SFAuthenticationManager::instance()->getCredentials()->getInstanceUrl();
}

void ApplicationUI::onSFOAuthFlowCanceled(SFOAuthInfo* info){
	qDebug()<<"[DEBUG] Authentication canceled";
	qDebug()<<"[DEBUG] The canceled authentication flow:"<<info->getAuthType();
}

void ApplicationUI::onSFOAuthFlowFailure(SFOAuthInfo* info){
	qDebug()<<"[DEBUG] Authentication failed";
	qDebug()<<"[DEBUG] The failed authentication flow:"<<info->getAuthType();
}

void ApplicationUI::onSFUserLoggedOut(){
	qDebug()<<"[DEBUG] Received user logged out signal";
	//recommended: automatically presents the login screen again
	SFAuthenticationManager::instance()->login();
}

void ApplicationUI::onSFLoginHostChanged(){
	qDebug()<<"[DEBUG] Received host changed signal";
	SFAuthenticationManager::instance()->login();
}
