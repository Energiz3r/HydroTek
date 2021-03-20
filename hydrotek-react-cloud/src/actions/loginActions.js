import { buildForDev } from '../config'

export const initLoginState = (
  {
    loggedInFacebook = false,
    loginError = false,
    loggedInApp = false,
    createAccount = false,
    responseErrorMessage = '',
    loggedIn = false,
    email = '',
    name = buildForDev ? 'Tim Eastwood' : ''
  } = {}
) => ({
  type: 'INIT_LOGIN_STATE',
  loginState: {
    loggedInFacebook,
    loginError,
    loggedInApp,
    createAccount,
    responseErrorMessage,
    loggedIn,
    email,
    name
  }
})
  
export const setLoginStatusFacebook = (status) => ({
  type: 'SET_LOGIN_STATUS_FACEBOOK',
  status
})

export const setLoginError = () => ({
  type: 'SET_LOGIN_ERROR'
})

export const setLoginStatusApp = (status) => ({
  type: 'SET_LOGIN_STATUS_APP',
  status
})
export const setLoginStatus = (status) => ({
  type: 'SET_LOGIN_STATUS',
  status
})

export const setCreateAccount = () => ({
  type: 'SET_CREATE_ACCOUNT'
})
export const setResponseErrorMessage = (message) => ({
  type: 'SET_RESPONSE_ERROR_MESSAGE',
  message
})
export const setUserDetails = (details) => ({
  type: 'SET_USER_DETAILS',
  details
})