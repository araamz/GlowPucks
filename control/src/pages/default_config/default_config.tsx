import { config } from "localforage"
import { useEffect, useState } from "react"
import default_config_styles from "./default_config.module.css";

export default function DefaultConfig() {

    // Get default config from server
    // Save it within the code state 
    // 

    const refresh_default_config_url = "http://127.0.0.1:5000/refresh_default_config"
    const default_config_url = "http://127.0.0.1:5000/default_config"
    const [config, set_config] = useState<any>()

    
    useEffect(() => {

        fetch(default_config_url).then(response => response.json())
        .then((data) => {
            set_config(JSON.stringify(data))
        })
    }, [])

    const fresh_button = () => {
        fetch(refresh_default_config_url)
    }

    return (
        <div className={default_config_styles.default_config}>
            <div className={default_config_styles.code_container}>
                <pre className={default_config_styles.code}>
                    <code>
                        { config }
                    </code>
                </pre>
            </div>
            <button className={default_config_styles.refresh_button} onClick={fresh_button}>
                Refresh
            </button>
        </div>
    )
}