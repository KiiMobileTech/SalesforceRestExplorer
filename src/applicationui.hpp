#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <bb/cascades/Container>
#include <bb/cascades/WebView>
#include "SFResult.h"
#include <QtNetwork/QNetworkAccessManager.h>
#include <bb/cascades/Label>
#include "SFOAuthInfo.h"
#include "SFAbstractApplicationUI.h"

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
    }
}

using namespace bb::cascades;
using namespace sf;

class QTranslator;
/*!
 * @brief Application object
 *
 *
 */

class ApplicationUI : public sf::SFAbstractApplicationUI
{
    Q_OBJECT
public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI() { }

private slots:
    void onSystemLanguageChanged();

    void onSFOAuthFlowSuccess(SFOAuthInfo* info);
    void onSFOAuthFlowCanceled(SFOAuthInfo* info);
    void onSFOAuthFlowFailure(SFOAuthInfo* info);

    void onSFUserLoggedOut();
    void onSFLoginHostChanged();

private:
    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;

};
#endif /* ApplicationUI_HPP_ */
