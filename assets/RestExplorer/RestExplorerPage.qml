import bb.cascades 1.2
import sf 1.0
import sf.rest 1.0

Page {
    id: restExplorerPage
    property string selectedMethodName : ""
    
    actionBarVisibility: ChromeVisibility.Default
    titleBar: TitleBar {
        title: "REST Explorer"
        kind: TitleBarKind.FreeForm
        scrollBehavior: TitleBarScrollBehavior.Sticky
        appearance: TitleBarAppearance.Default
        kindProperties: FreeFormTitleBarKindProperties {
            Container {
                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                leftPadding: 10
                rightPadding: 10
                Label {
                    text: "REST Explorer"
                    verticalAlignment: VerticalAlignment.Center
                    layoutProperties: StackLayoutProperties { spaceQuota: 1 }
                    textStyle {
                        color: Color.White
                        fontSize: FontSize.Medium
                        fontWeight: FontWeight.Bold
                    }
                }
                ActivityIndicator {
                    id: loadingIndicator;
                    objectName: "loadingIndicator";
                    verticalAlignment: VerticalAlignment.Center
                }
                SegmentedControl {
                    id: nativeOrQmlSegCtrl
                    objectName: "nativeOrQmlSegCtrl"
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Right
                    maxWidth: 0.0
                    options: [
                        Option {
                            text: "C++"
                            value: "native"
                        },
                        Option {
                            text: "QML"
                            value: "qml"
                            selected: true;
                        }
                    ] 
                }
            }
        }
    }
    
    actions: [
        ActionItem {
            id: invokeAction
            objectName: "invokeAction"
            title: "Start Test"
            imageSource: "asset:///RestExplorer/images/icon_done.png"
            ActionBar.placement: ActionBarPlacement.OnBar
        },
        ActionItem {
            id: resetAction
            objectName: "resetAction"
            title: "Clear All"
            imageSource: "asset:///RestExplorer/images/icon_clear.png"
            ActionBar.placement: ActionBarPlacement.OnBar
        },
        ActionItem {
            id: historyAction
            objectName: "historyAction"
            title: "Open History"
            imageSource: "asset:///RestExplorer/images/icon_history.png"
            ActionBar.placement: ActionBarPlacement.OnBar
        },
        ActionItem {
            id: clearHistoryAction
            objectName: "clearHistoryAction"
            title: "Clear History"
            imageSource: "asset:///RestExplorer/images/action_delete.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
        }, 
        ActionItem {
            id: revokeAccessAction
            objectName: "revokeAccessAction"
            title: "Revoke Access Token"
            imageSource: "asset:///RestExplorer/images/icon_risky.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
        }, 
        ActionItem {
            id: revokeAllAction
            objectName: "revokeAllAction"
            title: "Revoke All Tokens"
            imageSource: "asset:///RestExplorer/images/icon_risky.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
        }
    ]
    
    ScrollView {
        id: scrollView
        objectName: "scrollView"
        horizontalAlignment: HorizontalAlignment.Fill
        scrollViewProperties {
        	pinchToZoomEnabled: false
        	scrollMode: ScrollMode.Vertical
            overScrollEffectMode: OverScrollEffectMode.OnScroll
        }
        
        content: Container {
            horizontalAlignment: HorizontalAlignment.Fill 
            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                layout: DockLayout { }
                implicitLayoutAnimationsEnabled: false
                attachedObjects: [
                    LayoutUpdateHandler {
                        id: layoutHandler
                    }
                ]
                Button {
                    id: apiSelectorBtn
                    objectName: "apiSelectorBtn"
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Fill
                    implicitLayoutAnimationsEnabled: false;
                    minWidth: layoutHandler.layoutFrame.width;
                    maxWidth: layoutHandler.layoutFrame.width;
                }
                Container {
                    layout: DockLayout { }
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Fill
                    leftPadding: 10.0
                    rightPadding: 10.0
                    overlapTouchPolicy: OverlapTouchPolicy.Allow
                    Label {
                        text: "Select API"
                        overlapTouchPolicy: OverlapTouchPolicy.Allow
                        horizontalAlignment: HorizontalAlignment.Left
                    }
                    Label {
                        id: apiSelectedLbl
                        objectName: "apiNameLbl"
                        text: ""
                        overlapTouchPolicy: OverlapTouchPolicy.Allow
                        horizontalAlignment: HorizontalAlignment.Fill
                        textStyle.textAlign: TextAlign.Right
                    }
                }
            }
            
            Label {
                text: "API Signature:"
                textStyle.fontWeight: FontWeight.Bold
                implicitLayoutAnimationsEnabled: false;
            }
            
            Label {
                id: methodSignatureLbl
                objectName: "methodSignatureLbl";
                text: ""
                implicitLayoutAnimationsEnabled: false
                translationX: 10.0
            }
            
            Label {
                text: "Parameters:"
                textStyle.fontWeight: FontWeight.Bold
                implicitLayoutAnimationsEnabled: false;
            }
            
            Label {
                id: noneLbl
                text: "None"
                translationX: 10.0   
            }
            
            Container {
            	id: fieldsContainer
                objectName: "fieldsContainer"
                horizontalAlignment: HorizontalAlignment.Fill 
                implicitLayoutAnimationsEnabled: false  
                bottomMargin: 16.0 
                
                onControlAdded: {
                    noneLbl.visible = false;
                }
                
                onControlRemoved: {
                	noneLbl.visible = (fieldsContainer.count() == 0);
                }
            }
            
            Container {
                layout: StackLayout {orientation: LayoutOrientation.LeftToRight }
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
                implicitLayoutAnimationsEnabled: false
                topPadding: 8.0
                bottomPadding: 8.0
                visible: false
                Button {
                    id: invokeBtn
                    objectName: "invokeBtn"
                    text: "Invoke"
                }
                Button {
                    id: resetBtn
                    objectName: "resetBtn"
                    text: "Reset"
                }
            }
            
            Container {
                maxHeight: 2.0
                minHeight: 2.0
                horizontalAlignment: HorizontalAlignment.Fill
                background: Color.create("#ff808080")
                implicitLayoutAnimationsEnabled: false
            }
            
            Label {
                text: "Output:"
                textStyle.fontWeight: FontWeight.Bold
                implicitLayoutAnimationsEnabled: false;
            }
            
            TextArea {
                id: resultTextArea
                objectName: "resultTextArea"
                focusHighlightEnabled: true
                horizontalAlignment: HorizontalAlignment.Fill
                textStyle.textAlign: TextAlign.Left
                hintText: "Console"
                editable: false
                text: ""
                scrollMode: TextAreaScrollMode.Stiff
                inputMode: TextAreaInputMode.Default
                layoutProperties: StackLayoutProperties {
                    spaceQuota: -1.0
                }
                onTextChanged: {
                    scrollView.scrollToPoint(0, textLayoutHandler.layoutFrame.y,ScrollAnimation.Smooth);
                }
                attachedObjects: [
                    LayoutUpdateHandler {
                        id: textLayoutHandler
                    }
                ]
            }
        }
    }

    onCreationCompleted: {
        invokeBtn.clicked.connect(restExplorerPage.onInvokeButtonClicked);
        invokeAction.triggered.connect(restExplorerPage.onInvokeButtonClicked);
    }

    function onInvokeButtonClicked() {
        if (nativeOrQmlSegCtrl.selectedIndex == 0 || restExplorerPage.selectedMethodName.length == 0) {
            //native or method name is empty, we don't handle it here
            return;
        }
        loadingIndicator.running = true;
        var msg = ""; 
        resultTextArea.text = "";

        //construct evaluation string
        var evalCode = "SFRestAPI." + restExplorerPage.selectedMethodName + "(";
        var params = [];
        for (var i = 0; i < fieldsContainer.count(); i ++) {
            var paramControl = fieldsContainer.at(i);
            var value = paramControl.value();
            console.debug("param index=" + i + " value=" + value);
            params.push(value);
            if (i != 0) {
                evalCode += ", ";
            }
            evalCode += "params[" + i + "]";
        }
        evalCode += ")";

        //run eval code
        var result;
        try {
            console.debug("Eval Code: " + evalCode);
            result = eval(evalCode);
        } catch (e) {
            msg += "[QML] Error when calling API: " + e.message + "\n";
            console.log(msg);
            resultTextArea.text = resultTextArea.text + msg;
            loadingIndicator.running = false;
            return;
        }

		//deal with result
        if (result.toString().indexOf("SFRestRequest") >= 0) {
            //save path
            var text = result.path.toString() + "?";
            var params = result.requestParams;
            for(var k in params) {
                text += k + "=" + params[k] + "&"; 
            }
            pageController.saveCurrentFieldValues(text);
            
            //send SFRestRequest
	        msg += "[QML] Direct API Result: " + result.toString() + "\n";
            msg += "[QML] Sending request...\n";
            SFRestAPI.sendRestRequest(result, restExplorerPage, restExplorerPage.onTaskResultReady);
        } else {
            //save result
            pageController.saveCurrentFieldValues(result.toString());
            
            //other result, we assume it can be converted to string
            msg += "[QML] Direct API Result:\n" + result.toString() + "\n";
            loadingIndicator.running = false;
        }
        
        console.log(msg);
        resultTextArea.text = resultTextArea.text + msg;
    }

    function onTaskResultReady(result) {
        console.debug("[QML] API Response: " + result);
        var msg = "";
        if (! result.hasError) {
            msg = payloadDescription(result.payload);
            msg = "[QML] API Response: Code = %1 \nMessage: %2\nPayload:\n%3\n".arg(result.code).arg(result.message).arg(msg);
            console.debug(msg);
        } else {
            msg = "[QML] API Response: ErrorCode = %1 \nErrorMessage: %2\n".arg(result.code).arg(result.message);
            console.debug(msg);
        }

        resultTextArea.text = resultTextArea.text + msg;
        loadingIndicator.running = false;
    }

    function payloadDescription(payload) {
        var discription = payloadDescriptionRecursive('', payload, 0);
        return discription;
    }

    function payloadDescriptionRecursive(key, payload, depth) {
        var intent = "";
        var oneIntent = "      ";
        for (var i = 0; i < depth; i ++) {
            intent += oneIntent;
        }
        var description = intent;
        if (key.length != 0) {
            description += key + ': ';
        }
        if (payload instanceof Array) {
            description += '[\n';
            for (var k in payload) {
                description += payloadDescriptionRecursive('', payload[k], depth + 1) + ',\n';
            }
            description += intent + ']';
        } else if (payload instanceof Object) {
            description += '{\n';
            for (var k in payload) {
                description += payloadDescriptionRecursive(k, payload[k], depth + 1) + ',\n';
            }
            description += intent + '}';
        } else {
            description += payload;
        }
        return description;
    }
}
