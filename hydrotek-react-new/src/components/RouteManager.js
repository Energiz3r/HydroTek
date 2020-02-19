import { connect } from 'react-redux'
import { setInitialRoute, setRoute } from '../actions/UIActions'
import { serverAPILocation } from '../config'
import {
  setUserDetails,
  setLoginStatusApp,
  setLoginStatusFacebook,
  setLoginStatus
} from '../actions/loginActions'

class RouteManager extends React.Component {
    constructor(props) {
      super(props)
      this.state = {
        
      }
    }
    componentDidUpdate = (prevProps) => {
      const prev = prevProps.UI.route
      const next = this.props.UI.route
      if (prev != next) {
        this.props.history.push(next)
      }
    }
    componentDidMount = () => {
      const url = this.props.location.pathname
      this.props.dispatch(setInitialRoute(url))
      if (!this.props.login.loggedIn) {
        this.props.dispatch(setRoute('/login'))
      }
      fetch(serverAPILocation, {
        method: 'POST',
        cache: 'no-cache',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          "action": "checkLoginSimple"
        })
      })
        .then(result => result.json())
        .then(
          (result) => {
            if (result.loggedIn === "1") {
              console.log(result)
              this.props.dispatch(setUserDetails(result))
              this.props.dispatch(setLoginStatusApp(true))
              this.props.dispatch(setLoginStatusFacebook(true))
              setTimeout(()=>{
                this.props.dispatch(setLoginStatus(true)) // delay the login so the login modal can fade out
                this.props.dispatch(setRoute('/home'))
              }, 1000)
            } else {
              console.log("Failed simple login check:")
              console.log(result)
            }
          },
          (error) => {
            console.log("Error:")
            console.log(error)
          }
        )

    }
    render() {
      return null
    }
  }
  
  export default connect(state=>state)(RouteManager)