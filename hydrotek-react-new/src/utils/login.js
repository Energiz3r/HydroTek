import { store } from '../stores/store'
import {
  setLoginStatusFacebook,
  setLoginStatusApp,
  setLoginStatus,
  setCreateAccount,
  setUserDetails
} from '../actions/loginActions'
import { setRoute } from '../actions/UIActions'
import {serverAPILocation,stopAtLoginResponse, simulateCreateAccount} from '../config'
  
if (!window.serverData) { window.serverData = {} }

const appOutLoginRequest = () => {
  //console.log("Logging into to App...")
  fetch(serverAPILocation, {
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
            store.dispatch(setRoute('/home'))
          }, 200)
        } else {
          console.log(result)
          if (result.reason == 'no_app_user') {
            store.dispatch(setCreateAccount())
          } else if (result.reason == 'no_facebook_session') {
            store.dispatch(setResponseErrorMessage("Invalid facebook session! Please refresh the page and try again."))
          } else {
            console.log("App auth failed for some reason. Response:")
            console.log(result)
          }          
        }
      },
      (error) => {
        console.log("error:")
        console.log(error)
      }
    )
}
const facebookLoginCheck = (fbResponse) => {
  //console.log("Checking facebook auth with App...")
  fetch(serverAPILocation, {
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
          appOutLoginRequest()
          //console.log("App accepted facebook auth!")
        } else {
          console.log("App failed facebook auth:")
          console.log(result)
        }
      },
      (error) => {
        console.log("error:")
        console.log(error)
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
      console.log(response)
      store.dispatch(setLoginStatusFacebook(false))
      store.dispatch(setLoginStatusApp(false))
    }
  }  
  return false
}

export const dummyLogin = () => {
  setTimeout(()=>{
    store.dispatch(setLoginStatusFacebook(true))
    if (!stopAtLoginResponse) {
      store.dispatch(setLoginStatusApp(true))
    }
    if (simulateCreateAccount) {
      store.dispatch(setCreateAccount())
    }
    console.log("Logging in... (dummy mode)")
  }, 800)
  if (!stopAtLoginResponse) {
    setTimeout(()=>{
      console.log("Logged in! (dummy mode)")
      store.dispatch(setLoginStatus(true)) // delay the login so the login modal can fade out
      store.dispatch(setRoute('/home'))
    }, 1200)
  }
}