
import { LogViewer } from "@/components/LogViewer";

import logo from "@public/nx.ico";

export function App() {
  return (
    <main className="flex flex-col justify-center items-center p-8">
      <div className="flex justify-center items-center gap-8 mb-8">
        <img
          src={logo}
          alt="NX Logo"
          className="h-24 p-6 scale-120"
        />
        <h1 className="text-5xl font-bold my-4 leading-tight">Logs</h1>
      </div>

      <LogViewer />
    </main>
  );
}

export default App;
