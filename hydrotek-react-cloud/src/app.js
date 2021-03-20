import { Provider } from 'react-redux'
import 'normalize.css/normalize.css'
import './styles/styles.scss'
import AppMain from './components/AppMain'
import { facebookCallback, dummyLogin } from './utils/login.js'
import { store } from './stores/store'
import { buildForDev, facebookAppID } from './config'
import { BrowserRouter as Router } from 'react-router-dom'

if (!buildForDev) {

  console.log("Including Facebook API...")
  const script = document.createElement("script");
  script.src = "https://connect.facebook.net/en_GB/sdk.js#xfbml=1&version=v5.0&appId=" + facebookAppID + "&autoLogAppEvents=1";
  script.async = true;
  document.body.appendChild(script);

  console.log("Setting up Facebook login handler...")
  window.fbAsyncInit = function() {
    FB.init({
      appId: facebookAppID,
      cookie: true,  // enable cookies to allow the server to access the session
      xfbml: false,
      status: true,
      oauth: true,
      version: 'v3.2'
    })
    FB.Event.subscribe('auth.statusChange', function(response) {
      facebookCallback(response)
    })
  }.bind(this)
} else {
  console.log("Dev mode enabled: using dummy login, skipping load of Facebook assets.")
  //dummyLogin()
}


// browser detection
//var isOpera = (!!window.opr && !!opr.addons) || !!window.opera || navigator.userAgent.indexOf(' OPR/') >= 0; // Opera 8.0+
//var isFirefox = typeof InstallTrigger !== 'undefined'; // Firefox 1.0+
//var isSafari = /constructor/i.test(window.HTMLElement) || (function (p) { return p.toString() === "[object SafariRemoteNotification]"; })(!window['safari'] || (typeof safari !== 'undefined' && safari.pushNotification)); // Safari 3.0+ "[object HTMLElementConstructor]" 
const isIE = /*@cc_on!@*/false || !!document.documentMode; // Internet Explorer 6-11
//var isEdge = !isIE && !!window.StyleMedia; // Edge 20+
//var isChrome = !!window.chrome && !!window.chrome.webstore; // Chrome 1+
//var isBlink = (isChrome || isOpera) && !!window.CSS; // Blink engine detection

class ApplicationBase extends React.Component {
  constructor() {
    super()
    this.state = {
      displayMode: 'app'
    }
  }
  componentWillMount(){
    if (isIE) {
      this.setState({displayMode:'IE'})
    }
  }
  render() {
    const { displayMode } = this.state
    if (displayMode == 'app') {
      return (
        <Provider store={store}>
          <Router>            
            <AppMain />
          </Router>
        </Provider>
      )
    } else if (displayMode == 'IE') {
      return (
        <div className='noSupportContainer'>
          <h1>Sorry :(</h1>
          <h2>Your web browser, Internet Explorer, is not supported. Please download an alternative such as <a href="http://www.google.com/chrome">Google Chrome</a> or <a href="https://www.mozilla.org">Firefox</a>.</h2>
        </div>
      )
    }
  }
}

ReactDOM.render(<ApplicationBase />, document.getElementById('app'))
