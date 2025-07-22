

export default function Footer() {
  return (
    <footer className="bg-dark bg-gradient text-light text-center py-3 mt-5">
      <div className="container">
        <small>&copy; {new Date().getFullYear()} My Finance App. All rights reserved.</small>
      </div>
    </footer>
  );
}
