//import { serverLocation } from '../config'

//the default/initial state
export const initUIState = (
  {
    appIsBlurred = false,
    route = '',
    initialRoute = '',
    initialRouteRedirect = false
  } = {}
) => ({
  type: 'INIT_UI_STATE',
  UI: {
    appIsBlurred,
    route,
    initialRoute,
    initialRouteRedirect
  }
})

export const setRoute = (route, relative = false) => ({
  type: 'SET_ROUTE',
  route: relative ? route : (route)
})
export const setInitialRoute = (route) => ({
  type: 'SET_INITIAL_ROUTE',
  route: route
})
export const unsetInitialRoute = () => ({
  type: 'UNSET_INITIAL_ROUTE'
})