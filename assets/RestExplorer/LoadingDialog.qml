import bb.cascades 1.0

Dialog {
    property alias text: textLabel.text
    Container {
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }
        
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center
        background: Color.create(0.0, 0.0, 0.0, 0.7)
        focusPolicy: FocusPolicy.None
        leftPadding: 10.0
        rightPadding: 10.0
        topPadding: 10.0
        bottomPadding: 10.0
        ActivityIndicator {
            id: loadingIndicator
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            running: true
            minWidth: 100.0
            minHeight: 100.0
            preferredWidth: 120.0
            preferredHeight: 120.0
            maxWidth: 200.0
            maxHeight: 200.0
        }
        Label {
            id: textLabel
            text: qsTr("Loading...")
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            textStyle { 
                base: dialogTextStyle.style
            }
        }
    }
    onOpened: {
        // Reset the fire alarm since it is opened.
        loadingIndicator.running = true;
    }
    onClosed: {
        loadingIndicator.running = false;
    }
    
    attachedObjects: [
        TextStyleDefinition {
            id: dialogTextStyle
            base: SystemDefaults.TextStyles.PrimaryText
            textAlign: TextAlign.Center
            color: Color.White
            
            
        }
    ]
}
