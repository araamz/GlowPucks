import device_preview_styles from "./device_preview.module.css"; 

export default function DevicePreview() {

    return (
        <div className={device_preview_styles.device_preview}>
            <div className={device_preview_styles.label}>
                <p>
                    Puck 1
                </p>
            </div>
            <div className={device_preview_styles.graphic}>
                <p>
                    88%
                </p>
            </div>
            <div className={device_preview_styles.metadata}>
                <p>
                    ON
                </p>
            </div>
        </div>
    )

}