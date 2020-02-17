import { connect } from 'react-redux'
import { Route } from 'react-router-dom'
import { setRoute } from '../actions/UIActions'
import { serverLocation } from '../config'
import RouteManager from './RouteManager'
import LoginForm from './LoginForm'
import Navbar from './Navbar'
import Menu from './Menu'
import Home from './Home'
import Support from './Support'
import Listing from './Listing'

class AppMain extends React.Component {
  constructor(props) {
    super(props)
    this.state = {

    }
  }
  onMenuToggle = () => {
    this.setState({
      ...this.state,
      showMenu: !this.state.showMenu
    })
  }
  render() {
    const { appIsBlurred } = this.props.UI
    const { loggedIn } = this.props.login
    const { showMenu } = this.state
    return (
        <div className={"app-root-container"}>

          <Route path='/*' component={RouteManager} />
          <Route path={serverLocation + '/login'} component={LoginForm} />

          <div className={"app-overlay-container" + ((appIsBlurred || !loggedIn) ? " blur-container" : "")}>
            
            <div className='main-page-container'>
              <Navbar onMenuToggle={this.onMenuToggle} />
              <div className='secondary-page-container'>
                {showMenu && <Menu /> }
                <div className='main-content-container'>
                  <Route path={serverLocation + '/home'} component={Home} />
                  <Route path={serverLocation + '/listing/*'} component={Listing} />
                  <Route path={serverLocation + '/support'} component={Support} />
                </div>
              </div>
            </div>

            <div className={'app-color-overlay' + ((appIsBlurred || !loggedIn) ? ' app-color-overlay-visible' : ' app-color-overlay-invisible')}></div>
          </div>
        </div>
    )
  }
}

export default connect(state=>state)(AppMain)
