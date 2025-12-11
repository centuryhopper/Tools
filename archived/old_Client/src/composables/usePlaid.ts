// composables/usePlaid.ts
import axios from "axios";
import { jwtDecode } from "jwt-decode";
import Swal from "sweetalert2";
import { API_BASE_URL } from "../utils/utils";

export function usePlaid()
{
  const bankLink = async (
    bankAccountName: string,
    success: boolean = true,
  ): Promise<void> =>
  {
    const result = await Swal.fire({
      title: success ? "Success!" : "Failure...",
      text: success
        ? `Link to ${bankAccountName} successful!`
        : `Link to ${bankAccountName} unsuccessful :/`,
      icon: success ? "success" : "error",
      confirmButtonText: "OK",
    });

    if (result.isConfirmed)
    {
      location.reload()
    }
  };

  const linkPlaid = async () =>
  {
    const jwtToken =
      localStorage.getItem("token") ?? sessionStorage.getItem("token");
    if (!jwtToken)
    {
      console.error("JWT token not found");
      return;
    }

    const decoded = jwtDecode(jwtToken);
    // console.log(decoded);
    const userId = decoded.sub;
    const { data } = await axios.get(API_BASE_URL+"api/Plaid/get-link-token/" + userId, {
      headers: {
        Authorization: `Bearer ${jwtToken}`,
      },
    });

    //console.log("data:", data);

    const handler = window.Plaid.create({
      token: data.link_token,

      onSuccess: async (public_token, metadata) =>
      {
        // console.log("Plaid linked:", metadata);

        const jwtToken =
          localStorage.getItem("token") ?? sessionStorage.getItem("token");

        // console.log(jwtToken);

        const bankstuff = await axios.post(
          API_BASE_URL+"api/Plaid/exchange-public-token",
          { PublicToken: public_token },
          {
            headers: {
              Authorization: `Bearer ${jwtToken}`,
            },
          },
        );

        // console.log(bankstuff);

        await bankLink((bankstuff as any).data.name);
      },
      onExit: (err, metadata) =>
      {
        console.log("on exit is called");
        if (err) console.error("Plaid exited:", err);
        if (metadata) console.log("metadata:", metadata);
      },
    });

    handler.open();
  };

  return { linkPlaid };
}
