import { useState } from 'react'
import reactLogo from './assets/react.svg'
import './App.css'
import { Outlet } from 'react-router-dom'

function App() {

  return (
    <div className="App">
      <nav>
        
      </nav>
      <div>
        <Outlet />
      </div>
    </div>
  )
}

export default App
