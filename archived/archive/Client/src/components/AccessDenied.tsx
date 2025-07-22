export default function AccessDenied(
  message = "Please connect to at least one bank before accessing this page"
) {
  return <div className="m-5 p-5">{message}</div>;
}
