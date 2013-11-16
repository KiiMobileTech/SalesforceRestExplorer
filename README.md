SalesforceRestExplorer
======================

The SalesforceRestExplorer is an application that demonstrates the uses of the REST APIs found in Blackberry 10 SDK for Salesforce Mobile. This application requires the [Blackberry 10 SDK for Salesforce Mobile] (https://github.com/blackberry/BlackBerry10SDK-for-SalesforceMobile)

**Tested On**

BlackBerry Z10, Blackberry Dev Alpha C

**Author**

[Livan Yi Du] (https://github.com/stonedu)

## Getting Started
1. Clone the [Blackberry 10 SDK for Salesforce Mobile] (https://github.com/blackberry/BlackBerry10SDK-for-SalesforceMobile) and import the SalesforceSDK project into Momentics.
2. Clone the SalesforceRestExplorer repository and import the SalesforceRestExplorer project into Momentics
3. Go to Momentics's Preference->C/C++->Build->Environment, and click "Add". In the dialog, input SALESFORCE_SDK_LOC as the variable name and set the value to be the absolute path to your SalesforceSDK project. For example: /Workspace_Blackberry/git/SalesforceMobileSDK-BlackBerry10/SalesforceSDK
4. Build and run the SalesforceRestExporler project.

## More About Salesforce SDK
Please see the github page for [Blackberry 10 SDK for Salesforce Mobile] (https://github.com/blackberry/BlackBerry10SDK-for-SalesforceMobile) for more tutorials on how to use the SDK in your application.

## Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.