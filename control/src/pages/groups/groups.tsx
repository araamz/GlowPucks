import { useParams } from "react-router-dom"

export default function Group() {

    let { group_id } = useParams()

    return (
        <div>
            <p>
                Group Page { group_id }
            </p>
        </div>
    )

}