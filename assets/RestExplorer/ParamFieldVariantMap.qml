import bb.cascades 1.2

Container {
    id: control
    property string key : "params"
    property variant initialValue : {}
    function value() {
        //this function is to be called from C++
        var value = {}
        for(var i = 0; i < entryContainer.count(); i++) {
            var entryField = entryContainer.at(i);
            //we ignore empty key
            if (entryField.entryKey.length != 0) {
                value[entryField.entryKey] = entryField.entryValue;
            }
        }
        return value;
    }
    
    rightPadding: 10.0
    leftPadding: 15.0
    bottomMargin: 8.0
    implicitLayoutAnimationsEnabled: false
    Label {
        text: control.key
        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Left
        textStyle {
            textAlign: TextAlign.Left
            fontStyle: FontStyle.Italic
        }
    }
    
    Container {
        id: entryContainer
        implicitLayoutAnimationsEnabled: false
        
        function onInitialValueChanged() {
            var initalValue = control.initialValue;
            var container = entryContainer;
            container.removeAll();
            for(var k in initialValue) {
                var entry = entryDef.createObject();
                entry.entryKey = k;
                entry.entryValue = initialValue[k]; 
                container.add(entry);
            }
            //add additional line
            var newEntry = entryDef.createObject(); 
            newEntry.isLastRow = true;
            container.add(newEntry);
        }
        
        function insertEntryAfterField(field) {
            var index = entryContainer.indexOf(field);
            if (index < 0 ) {
                index = entryContainer.count() - 1;
            }
            field.isLastRow = false; 
            var entry = entryDef.createObject();
            entry.isLastRow = true;
            entryContainer.insert(index + 1,entry);
            entry.requestFocus();
        } 
        
        function removeEntryField(field) {
            var index = entryContainer.indexOf(field);
            if (index < 0 ) {
                return;
            }
            entryContainer.remove(field);
            if (index < entryContainer.count()) {
                entryContainer.at(index).requestFocus();
            }
        }
        
        attachedObjects: [
            ComponentDefinition {
                id: entryDef
                Container {
                    property bool isLastRow : false
                    property alias entryKey : keyField.text
                    property alias entryValue : valueField.text
                    id: entryField
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    bottomMargin: 8.0
                    implicitLayoutAnimationsEnabled: false
                    TextField {
                        id: keyField
                        horizontalAlignment: HorizontalAlignment.Right
                        verticalAlignment: VerticalAlignment.Center
                        hintText: "Key"
                        inputMode: TextFieldInputMode.Text
                        input {
                            submitKeyFocusBehavior: SubmitKeyFocusBehavior.Next
                            flags: TextInputFlag.SpellCheckOff | TextInputFlag.AutoCorrectionOff 
                            | TextInputFlag.AutoCapitalizationOff | TextInputFlag.AutoPeriodOff
                        }
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 2
                        }
                    }
                    Label {
                        text: ":"
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        textStyle.textAlign: TextAlign.Right
                    }
                    TextField {
                        id: valueField
                        horizontalAlignment: HorizontalAlignment.Right
                        verticalAlignment: VerticalAlignment.Center
                        hintText: "Value"
                        inputMode: TextFieldInputMode.Text
                        input {
                            submitKeyFocusBehavior: SubmitKeyFocusBehavior.Next
                            flags: TextInputFlag.SpellCheckOff | TextInputFlag.AutoCorrectionOff | TextInputFlag.AutoCapitalizationOff | TextInputFlag.AutoPeriodOff
                            onSubmitted: {
                                if (!entryField.isLastRow || valueField.text.length == 0 || keyField.text.length == 0) {
                                    return;
                                }
                                //last row losing focus and there is value, auto add another row
                                entryContainer.insertEntryAfterField(entryField);    
                            }
                        }
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 3
                        }
                    }
                    ImageButton {
                        id: minusBtn
                        defaultImageSource: "asset:///RestExplorer/images/icon_minus.png"
                        pressedImageSource: "asset:///RestExplorer/images/icon_minus_pressed.png"
                        horizontalAlignment: HorizontalAlignment.Right
                        verticalAlignment: VerticalAlignment.Center
                        minWidth: 54.0
                        minHeight: 54.0
                        visible: !entryField.isLastRow
                        onClicked: entryContainer.removeEntryField(entryField);
                    }
                    ImageButton {
                        id: addBtn
                        defaultImageSource: "asset:///RestExplorer/images/icon_plus.png"
                        pressedImageSource: "asset:///RestExplorer/images/icon_plus_pressed.png"
                        horizontalAlignment: HorizontalAlignment.Right
                        verticalAlignment: VerticalAlignment.Center
                        minWidth: 54.0
                        minHeight: 54.0
                        visible: entryField.isLastRow
                        onClicked: entryContainer.insertEntryAfterField(entryField);
                    }
                    
                    function requestFocus() {
                        keyField.requestFocus();
                    }
                }
            }
        ]
    }
    
    onCreationCompleted: {
        control.initialValueChanged.connect(entryContainer.onInitialValueChanged);
        entryContainer.onInitialValueChanged();
    }
}
