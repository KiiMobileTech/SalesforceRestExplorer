import bb.cascades 1.2
import "./RestExplorer"
import "./salesforce"
import sf 1.0
import sf.rest 1.0

TabbedPane {
    id: tabbedPane
    property Sheet settingsSheet;
    showTabsOnActionBar: false
    
    Menu.definition: MenuDefinition {        
        // Add any remaining actions
        actions: [
            ActionItem {
                title: "Logout"
                imageSource: "asset:///images/icon_logout.png"
                onTriggered: {
                    console.debug("logout action triggered");
                    SFAuthenticationManager.logout();
                }
            }
        ]
        settingsAction: SettingsActionItem {
            onTriggered: {
                console.debug("setting action triggered");
                if (!settingsSheet){
                    settingsSheet = settingsSheetDef.createObject(tabbedPane);
                }
                settingsSheet.open();
            }
        }
        helpAction: HelpActionItem {
            onTriggered: {
                console.debug("help action triggered");
            }
        }
        //Tim: this is for the setting page
        attachedObjects: [
            ComponentDefinition {
                id: settingsSheetDef
                Sheet {
                    id: settingsSheet
                    peekEnabled: false; 
                    content: SFSettings { 
                        titleBar: TitleBar {
                            title: qsTr("Settings")
                            dismissAction: ActionItem {
                                title: "Back"
                                imageSource: "asset:///images/ic_previous.png"
                                onTriggered: {
                                    settingsSheet.close();
                                }
                            }
                        } 
                        
                        paneProperties: NavigationPaneProperties {
                            backButton: ActionItem {
                                title: "Back"
                            }
                        }
                    } 
                    //Tim: settings page is created new every time, so destroy it when it's popped
                    onClosed: {
                        console.debug("destroy settings page");
                        settingsSheet.destroy();
                        SFAccountManager.updateLoginHost();
                    }
                }
            }
        ]
    }
    
    Tab {
        id: restTab
        objectName: "restTab"
        title: qsTr("REST Explorer")
        imageSource: "asset:///RestExplorer/images/icon_cube.png"
        content: NavigationPane {
            id:restNav
        }
        onCreationCompleted: {
            tabbedPane.showTabsOnActionBar = false;
            if (!restControllerDef.created) {
                var restController= restControllerDef.createObject();
                restController.load("RestExplorer/RestExplorerPage.qml");
                restNav.push(restController.root);
                restControllerDef.created = true;
            }
        }
        
        attachedObjects: [
            ComponentDefinition {
                id: restControllerDef
                property bool created: false
                RESTExplorerPageController { }
            }
        ]
    }
}