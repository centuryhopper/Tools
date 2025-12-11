import Swal from "sweetalert2";
import { useTheme } from "../stores/theme-store";


export default function useSweetAlertPopups()
{
    const { isDark } = useTheme()
    const showFeedbackPopup = async (
        { success = true,
            successMsg = '',
            failMsg = '', }
            : {
                success?: boolean;
                successMsg?: string;
                failMsg?: string;
            }
    ): Promise<void> =>
    {
        const result = await Swal.fire({
            title: success ? "Success!" : "Failure...",
            text: success
                ? successMsg
                : failMsg,
            icon: success ? "success" : "error",
            confirmButtonText: "OK",
            customClass: {
                popup: isDark.value ? "swal-dark" : "",
            },
            allowOutsideClick: false,
        });
    };

    const confirmationPopup = async (
        title: string = "Are you sure?",
        text: string = "Do you want to proceed?",
        confirmedCb: () => Promise<void>,
        dismissedCb: () => Promise<void>,
    ): Promise<void> =>
    {
        const result = await Swal.fire({
            title,
            text,
            icon: "question",
            showCancelButton: true,
            confirmButtonText: "Yes",
            cancelButtonText: "No",
            confirmButtonColor: '#28a745',
            cancelButtonColor: 'red',
            customClass: {
                popup: isDark.value ? "swal-dark" : "",
            },
            allowOutsideClick: false,
        });

        if (result.isConfirmed)
        {
            confirmedCb()
        }
        else if (result.isDismissed)
        {
            dismissedCb()
        }
    };

    return {
        showFeedbackPopup,
        confirmationPopup,
    }
}