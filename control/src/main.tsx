import React from 'react'
import ReactDOM from 'react-dom/client'
import App from './App'
import './index.css'

import {
  createBrowserRouter,
  RouterProvider,
  Route,
  BrowserRouter,
} from "react-router-dom";

import Device from './pages/device/device';
import Group from './pages/groups/groups';
import Home from './pages/home/home';
import DefaultConfig from './pages/default_config/default_config';

const router = createBrowserRouter([
  {
    path: "/",
    element: <App />,
    children: [
      {
        path: "/",
        element: <Home />
      },
      {
        path: "devices/:device_uuid",
        element: <Device />
      },
      {
        path: "groups/:group_uuid",
        element: <Group />
      },
      {
        path: "/default_config",
        element: <DefaultConfig />
      }
    ]
  }
]);

ReactDOM.createRoot(document.getElementById('root') as HTMLElement).render(
  <React.StrictMode>
    <RouterProvider router={router} />
  </React.StrictMode>
)
