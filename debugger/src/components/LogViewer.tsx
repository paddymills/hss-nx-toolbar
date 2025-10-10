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

  if (messages.length === 0) {
    return <div>No logs yet</div>;
  }

  return (
    <div className="flex flex-col justify-center-safe items-center-safe w-full lg:w-[80%]">
      {messages.map((msg, index) => (
        <LogItem key={index} {...msg} />
      ))}
    </div>
  );
}
