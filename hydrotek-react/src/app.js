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
      //loggedIn: true
    }
  }
  render = () => {
    const loc = this.props.ui.location    
    return (
      <div>
        <Header isMounted={true} isHeader={true} />
        <HomePage isMounted={this.state.loggedIn && loc == 'home'} />
        <LoginPage isMounted={!this.state.loggedIn} />
        <Information isMounted={this.state.loggedIn && loc == 'information'} />
        <Charts isMounted={this.state.loggedIn && loc == 'Charts'} />
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