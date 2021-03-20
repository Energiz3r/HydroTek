const reducerDefaultState = []

export default (state = reducerDefaultState, action) => {
  switch (action.type) {
    case 'INIT_LOGIN_STATE':
      return action.loginState
    case 'SET_LOGIN_STATUS_FACEBOOK':
      return {
        ...state,
        loggedInFacebook: action.status 
      }
    case 'SET_LOGIN_ERROR':
      return {
        ...state,
        loginError: true
      }
    case 'SET_LOGIN_STATUS_APP':
      return {
        ...state,
        loggedInApp: action.status 
      }
    case 'SET_LOGIN_STATUS':
        return {
          ...state,
          loggedIn: action.status,
          createAccount: action.status ? false : state.createAccount,
          responseErrorMessage: action.status ? '' : state.responseErrorMessage
        }
    case 'SET_CREATE_ACCOUNT':
      return {
        ...state,
        createAccount: true
      }
    case 'SET_RESPONSE_ERROR_MESSAGE':
      return {
        ...state,
        responseErrorMessage: action.message
      }
    case 'SET_USER_DETAILS':
      return {
        ...state,
        email: action.details.email,
        name: action.details.name
      }
    default:
      return state
  }
};