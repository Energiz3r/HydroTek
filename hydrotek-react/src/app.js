import React from 'react'
import ReactDOM from 'react-dom'
import 'normalize.css/normalize.css'
import './styles/styles.scss'
import { Provider } from 'react-redux'
import store from './store'
import { connect } from 'react-redux'

import Header from './components/Header'
import HomePage from './components/HomePage'
import Information from './components/Information'
import LoginPage from './components/Login'
import Charts from './components/Charts'

class Content extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      loggedIn: window.loggedIn == 1 ? true : false
    }
  }
  render = () => {
    const { loggedIn } = this.state
    const { location } = this.props.ui
    return (
      <div>
        <Header isMounted={true} isHeader={true} />
        <LoginPage isMounted={!loggedIn} />
        <HomePage isMounted={loggedIn && location == 'home'} />
        <Information isMounted={location == 'information'} />
        <Charts isMounted={loggedIn && location == 'Charts'} />
      </div>
    )
  }
}

const ContentConnected = connect(state=>state)(Content)
class DefaultComponent extends React.Component {
  render = () => {
    return (
      <Provider store={store}>
        <ContentConnected />
      </Provider>
    )
  }
}

ReactDOM.render(<DefaultComponent />, document.getElementById('app'))
