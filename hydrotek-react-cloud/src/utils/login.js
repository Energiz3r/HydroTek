import { store } from '../stores/store'
import {
  setLoginStatusFacebook,
  setLoginStatusApp,
  setLoginStatus,
  setLoginError,
  setCreateAccount,
  setUserDetails
} from '../actions/loginActions'
import { setRoute } from '../actions/UIActions'
import { serverAPIEndpointFacebook } from '../config'
  
if (!window.serverData) { window.serverData = {} } //get data output by PHP

const appLoginRequest = () => {
  console.log("Logging into to App...")
  console.log(serverAPIEndpointFacebook)
  fetch(serverAPIEndpointFacebook, {
    method: 'POST',
    cache: 'no-cache',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({
      "action": "appLogin"
    })
  })
    //.then((res) => { res.text().then(function (text) { console.log(text) }) }) //debug output from api.php
    .then(result => result.json())
    .then(
      (result) => {
        if (result.loggedIn === "1") {
          //console.log(result)
          console.log("App auth successful!")
          store.dispatch(setLoginStatusApp(true))
          setTimeout(()=>{
            store.dispatch(setLoginStatus(true)) // delay the login so the login modal can fade out
            store.dispatch(setRoute('home'))
          }, 200)
        } else {
          console.log(result)
          if (result.reason == 'no_app_user') {
            store.dispatch(setCreateAccount())
          } else if (result.reason == 'no_facebook_session') {
            store.dispatch(setResponseErrorMessage("Invalid facebook session! Please refresh the page and try again."))
          } else {
            store.dispatch(setLoginError())
            console.log("App auth failed for some reason. Response:")
            console.log(result)
          }          
        }
      },
      (error) => {
        store.dispatch(setLoginError())
        console.log("Error checking login state with app:")
        console.log(error)
      }
    )
}
const facebookLoginCheck = (fbResponse) => {
  console.log("Checking facebook auth with App...")
  console.log(serverAPIEndpointFacebook)
  fetch(serverAPIEndpointFacebook, {
    method: 'POST',
    cache: 'no-cache',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({
      "action": "facebookLogin",
      "fb_access_token": fbResponse.accessToken
    })
  })
    //.then((res) => { res.text().then(function (text) { console.log(text) }) }) //debug output from api.php
    .then(result => result.json())
    .then(
      (result) => {
        if (result.loggedIn === "1") {
          console.log(result)
          store.dispatch(setUserDetails(result))
          appLoginRequest()
          //console.log("App accepted facebook auth!")
        } else {
          store.dispatch(setLoginError())
          console.log("App failed facebook auth:")
          console.log(result)
        }
      },
      (error, result) => {
        store.dispatch(setLoginError())
        console.log("Login error while checking facebook login status with server:")
        console.log(error)
        console.log(result)
      }
    )
}

export const facebookCallback = (response) => {
  console.log("Facebook auth response received...")
  console.log(response)
  if (!store.getState().login.loggedIn) {
    if (response.status == 'connected') {
      setTimeout(()=>{
        store.dispatch(setLoginStatusFacebook(true))
        facebookLoginCheck(response.authResponse)
      },1000)
    } else {
      store.dispatch(setLoginError())
      console.log("Failed to confirm facebook callback:")
      console.log(response)
      store.dispatch(setLoginStatusFacebook(false))
      store.dispatch(setLoginStatusApp(false))
    }
  }  
  return false
}
