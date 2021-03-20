import { connect } from 'react-redux'
import { appName, buildForDev } from '../config'

class Navbar extends React.Component {
  constructor(props) {
    super(props)
    this.state = {

    }
  }
  onMenuClick = () => {
    this.props.onMenuToggle()
  }
  render() {
    return (
      <div className="navbar-container">
        <button onClick={this.onMenuClick} className='button-default navbar-button navbar-burger-button'><i className="fas fa-bars fa-navbar"></i></button>
        <h2>{appName}</h2>
        <h4 className='navbar-name'>{this.props.login.name}</h4>
        {this.props.loggedInFacebook && this.props.loggedIn && !webpackDevServer &&
          <div
            className="fb-login-button"
            data-width=""
            data-size="large"
            data-button-type="continue_with"
            data-auto-logout-link="true"
            data-use-continue-as="true"
            >
          </div>}
      </div>
    )
  }
}

export default connect(state=>state)(Navbar)