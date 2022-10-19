import { useParams } from "react-router-dom"

export default function Device() {

    let { device_id } = useParams()

    return (
        <div>
            <p>
                Device Page { device_id }
            </p>
        </div>
    )

}