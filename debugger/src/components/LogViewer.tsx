import { useState, useEffect } from "react";
import type { Message } from "@/lib/types";
import { LogItem } from "@/components/LogItem";

export function LogViewer() {
  const [messages, setMessages] = useState<Message[]>([]);
  useEffect(() => {
    const ws = new WebSocket(`ws://${window.location.host}`);

    ws.onmessage = (event) => {
      const msg = JSON.parse(event.data);
      console.debug(msg);

      setMessages((msgs) => [...msgs, msg]);
    };

    // cleanup on unmount
    return () => ws.close();
  }, [setMessages]);
  const clearMessages = async () => {
    await fetch("/clear", { method: "POST" });
    setMessages([]);
  }

  if (messages.length === 0) {
    return <div>No logs yet</div>;
  }

  return (
    <div className="flex flex-col justify-center-safe items-center-safe w-full lg:w-[80%]">
      <button
        className="lg:mx-[20%] mb-2 p-2 bg-linear-to-r/shorter from-indigo-500 via-rose-500 to-indigo-500 text-white rounded-full"
        onClick={clearMessages}
      >
        Clear
      </button>
      {messages.map((msg, index) => (
        <LogItem key={index} {...msg} />
      ))}
    </div>
  );
}
