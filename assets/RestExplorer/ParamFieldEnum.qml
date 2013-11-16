import bb.cascades 1.2

Container {
    id: control
    property string key : "objectType"
    property int initialValue : -1
    property variant  options : [["Option1", 1], ["Option2", 2], 0]
    function value() {
        //this function is to be called from C++
        return valueSegCtrl.selectedValue;
    }
    
    rightPadding: 10.0
    leftPadding: 15.0
    bottomMargin: 8.0
    Container {
        layout: StackLayout { orientation: LayoutOrientation.TopToBottom }
        
        Label {
            id: keyLbl
            objectName:"keyLbl"
            text: control.key
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Left
            textStyle {
                textAlign: TextAlign.Left
                fontStyle: FontStyle.Italic
            }
        }
        SegmentedControl {
            id: valueSegCtrl
            horizontalAlignment: HorizontalAlignment.Center
            accessibility.name: "Segments" 
            
            function onOptionsChanged() {
                var options = control.options;
                valueSegCtrl.removeAll();
                for(var k in options) {
                    var pair = options[k];
                    if (!(pair instanceof Array) || pair.length != 2) {
                        continue;
                    }
                    
                    var option = optionDef.createObject(valueSegCtrl);
                    option.text = pair[0];
                    option.value = pair[1]; 
                    valueSegCtrl.add(option)
                }
            }
            
            function onInitialValueChanged() {
                for(var i=0; i<valueSegCtrl.count();i++) {
                    var option = valueSegCtrl.at(i);
                    if (option.value == control.initialValue) {
                        valueSegCtrl.selectedOption = option;
                        return;
                    }
                }
                valueSegCtrl.selectedIndex = -1; 
            }
            attachedObjects: [
                ComponentDefinition {
                    id: optionDef 
                    Option { }
                }
            ]
            
        }
    }
    
    onCreationCompleted: {
        control.optionsChanged.connect(valueSegCtrl.onOptionsChanged);
        control.initialValueChanged.connect(valueSegCtrl.onInitialValueChanged);
    }
}
