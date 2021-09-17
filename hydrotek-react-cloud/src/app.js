import { Provider } from "react-redux";
import "normalize.css/normalize.css";
import "./styles/styles.scss";
import AppMain from "./components/AppMain";
import { facebookCallback, dummyLogin } from "./utils/login.js";
import { store } from "./stores/store";
import { buildForDev, facebookAppID } from "./config";
import { BrowserRouter as Router } from "react-router-dom";
import { ThemeProvider } from "react-jss";
import { theme } from "./theme";

if (!buildForDev) {
  console.log("Including Facebook API...");
  const script = document.createElement("script");
  script.src =
    "https://connect.facebook.net/en_GB/sdk.js#xfbml=1&version=v5.0&appId=" +
    facebookAppID +
    "&autoLogAppEvents=1";
  script.async = true;
  document.body.appendChild(script);

  console.log("Setting up Facebook login handler...");
  window.fbAsyncInit = function () {
    FB.init({
      appId: facebookAppID,
      cookie: true, // enable cookies to allow the server to access the session
      xfbml: false,
      status: true,
      oauth: true,
      version: "v3.2",
    });
    FB.Event.subscribe("auth.statusChange", function (response) {
      facebookCallback(response);
    });
  }.bind(this);
} else {
  console.log(
    "Dev mode enabled: using dummy login, skipping load of Facebook assets."
  );
  //dummyLogin()
}

class ApplicationBase extends React.Component {
  render() {
    return (
      <Provider store={store}>
        <Router>
          <ThemeProvider theme={theme}>
            <AppMain />
          </ThemeProvider>
        </Router>
      </Provider>
    );
  }
}

ReactDOM.render(<ApplicationBase />, document.getElementById("app"));
