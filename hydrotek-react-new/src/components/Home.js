import { connect } from 'react-redux'
import { Link } from 'react-router-dom'
import { serverLocation } from '../config'

class Home extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      showMenu: false
    }
  }
  render() {
    const {initialRoute,initialRouteRedirect} = this.props.UI
    return (
      <div>
        
        <p>Home Page</p>
        <p>Initial Route: {initialRoute}</p>
        <p>Initial Set: {initialRouteRedirect ? 'yes' : 'no'}</p>
        <Link to={serverLocation + "/devices"}>
          <p>Devices</p>
        </Link>
      </div>
    )
  }
}

export default connect(state=>state)(Home)