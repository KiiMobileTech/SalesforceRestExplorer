import bb.cascades 1.2

Container {
    id: control
    property string key : "objectType"
    property string initialValue : "value"
    function value() {
        //this function is to be called from C++
        return valueField.text;
    }
    
    rightPadding: 10.0
    leftPadding: 15.0
    bottomMargin: 8.0
    Container {
        layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
        
        Label {
            id: keyLbl
            objectName:"keyLbl"
            text: control.key
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Right
            textStyle {
                textAlign: TextAlign.Left
                fontStyle: FontStyle.Italic
            }
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
        }
        TextField {
            id: valueField
            objectName: "valueField"
            horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Center
            text: ""
            hintText: "String" 
            inputMode: TextFieldInputMode.Text
            input {
                submitKeyFocusBehavior: SubmitKeyFocusBehavior.Next
                flags: TextInputFlag.SpellCheckOff | TextInputFlag.AutoCorrectionOff | TextInputFlag.AutoCapitalizationOff | TextInputFlag.AutoPeriodOff
            }
            
            layoutProperties: StackLayoutProperties {
                spaceQuota: 3
            }
        }
    }
    
    onInitialValueChanged: {
        valueField.text = initialValue;
    }
    
    onCreationCompleted: {
        valueField.text = initialValue;
    }
}
