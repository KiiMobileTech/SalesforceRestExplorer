import bb.cascades 1.2

Dialog {
    id: listDialog
    property alias title: textLabel.text
    property variant selectedIndexPath : []
    property bool autoDismiss : false
    Container {
        topPadding: 10.0
        bottomPadding: 10.0
        leftPadding: 30.0
        rightPadding: 30.0
        Container {
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
            
            layout: DockLayout {
            
            }
            
            ImageView {
                id: backgroundImage
                imageSource: "asset:///RestExplorer/images/popup_bg.amd"
                verticalAlignment: VerticalAlignment.Fill
                horizontalAlignment: HorizontalAlignment.Fill
            
            }
            Container {
                id: bordersLayerContainer
                layout: DockLayout {
                
                }
                verticalAlignment: VerticalAlignment.Fill
                horizontalAlignment: HorizontalAlignment.Fill
                topPadding: 143.0
                bottomPadding: 148.0
                ImageView {
                    imageSource: "asset:///RestExplorer/images/divider_top.png"
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Top
                    scalingMethod: ScalingMethod.AspectFit
                }
                
                ImageView {
                    imageSource: "asset:///RestExplorer/images/divider_bottom.png"
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Bottom
                    scalingMethod: ScalingMethod.AspectFit
                }
            }
            
            Container {
                id: contentContainer
                verticalAlignment: VerticalAlignment.Fill
                horizontalAlignment: HorizontalAlignment.Fill
                leftPadding: 50.0
                rightPadding: 50.0
                layout: DockLayout {
                
                }
                Container {
                    id: titleContainer
                    verticalAlignment: VerticalAlignment.Top
                    horizontalAlignment: HorizontalAlignment.Fill
                    topPadding: 43.0
                    Label {
                        id: textLabel
                        horizontalAlignment: HorizontalAlignment.Center
                        text: qsTr("Title")
                        textStyle {
                            base: dialogTitleStyle.style
                        }
                    }
                }
                Container {
                    id: mainContentContainer
                    topPadding: bordersLayerContainer.topPadding + 24.0
                    bottomPadding: bordersLayerContainer.bottomPadding + 24.0
                    verticalAlignment: VerticalAlignment.Fill
                    horizontalAlignment: HorizontalAlignment.Fill
                    
                    ListView {
                        id: theList
                        objectName: "list"
                        layout: StackListLayout {
                            headerMode: ListHeaderMode.Sticky
                        }
                        
                        listItemComponents: [
                            ListItemComponent {
                                type: "header"
                                content: Container {
                                    layout: DockLayout { }
                                    Container {
                                        layout: StackLayout {
                                            orientation: LayoutOrientation.LeftToRight
                                        }
                                        
                                        verticalAlignment: VerticalAlignment.Fill
                                        horizontalAlignment: HorizontalAlignment.Fill
                                        leftPadding: 10.0
                                        rightPadding: 15.0
                                        background:Color.create(0.5, 0.5, 0.5, 0.8)
                                        Label {
                                            verticalAlignment: VerticalAlignment.Center
                                            horizontalAlignment: HorizontalAlignment.Fill
                                            minHeight: 22.0
                                            maxHeight: 44.0
                                            text: (typeof ListItemData.text  == 'undefined') ? ListItemData : ListItemData.text 
                                            layoutProperties: StackLayoutProperties {
                                                spaceQuota: 1.0
                                            }
                                            textStyle {
                                                base: SystemDefaults.TextStyles.BodyText
                                                textAlign: TextAlign.Left
                                                color: Color.White
                                            }
                                        }
                                    }
                                }
                            },
                            ListItemComponent {
                                type: "item"
                                content: Container {
                                    id:listCell
                                    property bool hasSubText : (!(typeof ListItemData.subtext == 'undefined') && ListItemData.subtext.length != 0)
                                    minHeight: hasSubText ? 110.0 : 88.0
                                    maxHeight: hasSubText ? 110.0 : 88.0
                                    background: ListItem.active ? Color.Gray : Color.Transparent
                                    layout: DockLayout { }
                                    Container {
                                        layout: StackLayout {
                                            orientation: LayoutOrientation.LeftToRight
                                        }
                                        verticalAlignment: VerticalAlignment.Fill
                                        horizontalAlignment: HorizontalAlignment.Fill
                                        leftPadding: 10.0
                                        rightPadding: 15.0
                                        Container {
                                            verticalAlignment: VerticalAlignment.Center
                                            layoutProperties: StackLayoutProperties {
                                                spaceQuota: 1.0
                                            }

                                            Label {
                                                text: ListItemData.text
                                                verticalAlignment: VerticalAlignment.Center
                                                horizontalAlignment: HorizontalAlignment.Fill
                                                textStyle {
                                                    base: listTextStyle.style
                                                }
                                                textFit {
                                                    mode: LabelTextFitMode.FitToBounds
                                                }
                                            }
                                            
                                            Label {
                                                text: listCell.hasSubText ? ListItemData.subtext : ""
                                                verticalAlignment: VerticalAlignment.Center
                                                horizontalAlignment: HorizontalAlignment.Fill
                                                visible: listCell.hasSubText
                                                textStyle {
                                                    base: listSubTextStyle.style
                                                }
                                                textFit {
                                                    mode: LabelTextFitMode.FitToBounds
                                                }
                                            }
                                        }
                                        
                                        ImageView {
                                            imageSource: listCell.ListItem.selected ? "asset:///RestExplorer/images/radio_on.png" : "asset:///RestExplorer/images/radio_off.png"
                                            
                                            verticalAlignment: VerticalAlignment.Center
                                            minWidth: 48.0
                                            minHeight: 48.0
                                        }
                                    }
                                    Divider {
                                        verticalAlignment: VerticalAlignment.Bottom
                                        horizontalAlignment: HorizontalAlignment.Fill
                                    }
                                    attachedObjects: [
                                        TextStyleDefinition {
                                            id: listTextStyle
                                            base: SystemDefaults.TextStyles.BodyText
                                            textAlign: TextAlign.Left
                                            color: Color.White
                                            fontSize: FontSize.Medium
                                        }, 
                                        TextStyleDefinition {
                                            id: listSubTextStyle
                                            base: SystemDefaults.TextStyles.BodyText
                                            textAlign: TextAlign.Left
                                            fontWeight: FontWeight.W100
                                            color: Color.LightGray
                                            fontStyle: FontStyle.Italic
                                            fontSize: FontSize.Small
                                        }
                                    ]
                                }
                            }
                        ]
                        onTriggered: {
                            theList.select(listDialog.selectedIndexPath, false);
                            theList.select(indexPath, true);
                            listDialog.selectedIndexPath = indexPath;
                            if (listDialog.autoDismiss) {
                                listDialog.close();
                            }
                        }
                        
                    }
                }
                Container {
                    layout: DockLayout { }
                    id: buttonsContainer
                    verticalAlignment: VerticalAlignment.Bottom
                    horizontalAlignment: HorizontalAlignment.Fill
                    topPadding: 20.0
                    bottomPadding: 40.0
                    maxHeight: bordersLayerContainer.bottomPadding
                    minHeight: bordersLayerContainer.bottomPadding
                    Button {
                        text: "Cancel"
                        verticalAlignment: VerticalAlignment.Fill
                        horizontalAlignment: HorizontalAlignment.Left
                        maxWidth: 270.0
                        onClicked: {
                            listDialog.selectedIndexPath = [];
                            listDialog.close();
                        }
                    }
                    Button {
                        text: "Ok"
                        verticalAlignment: VerticalAlignment.Fill
                        horizontalAlignment: HorizontalAlignment.Right
                        maxWidth: 270.0
                        visible: !listDialog.autoDismiss
                        onClicked: {
                            listDialog.close();
                        }
                    }
                }
            }
        }
    }
    onOpened: {
        //select the initial indexPath
        theList.select(listDialog.selectedIndexPath, true);
        theList.scrollToItem(listDialog.selectedIndexPath);
    }
    onClosed: {
    
    }
    
    onCreationCompleted: {
        theList.select(listDialog.selectedIndexPath, true);
    }
    
    attachedObjects: [
        TextStyleDefinition {
            id: dialogTitleStyle
            base: SystemDefaults.TextStyles.TitleText
            textAlign: TextAlign.Center
            color: Color.White
        }
    ]
}
