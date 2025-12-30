import type { Message } from "@/lib/types";
import { useMemo, useState } from "react";
import { twMerge } from "tailwind-merge";

const TODAY = new Date(Date.now()).getDate();

const levelColor = (level: string) => {
  switch (level) {
    case "CRITICAL":
      // TODO: purple?
      return "bg-purple-100 text-purple-800";
    case "ERROR":
      return "bg-red-100 text-red-800";
    case "WARNING":
      return "bg-yellow-100 text-yellow-800";
    case "DEBUG":
      return "bg-green-100 text-green-800";
    case "INFO":
    default:
      return "bg-blue-100 text-blue-800";
  }
};

function LogLevelChip({
  level,
  className,
}: {
  level: string;
  className?: string;
}) {
  const defaultStyles =
    "uppercase py-1 w-[8ch] rounded-md font-bold text-sm text-center";

  return (
    <span className={twMerge(levelColor(level), defaultStyles, className)}>
      {level}
    </span>
  );
}

interface LogItemProps extends Message {
  showDate?: boolean;
}

export function LogItem(props: LogItemProps) {
  const {
    timestamp,
    level,
    message,
    partFile,
    filename,
    lineno,
    showDate = false,
    traceback,
  } = props;

  const [showModal, setShowModal] = useState(false);

  const dateFmt = useMemo(() => {
    const date = new Date(timestamp);

    if (date.getDate() === TODAY) {
      return date.toLocaleTimeString();
    }

    return date.toLocaleString();
  }, [timestamp]);

  return (
    <>
      <div className="flex gap-2 mb-2 p-2 font-mono bordered cursor-pointer hover:bg-gray-100/10">
        <LogLevelChip level={level} />
        {showDate ? <strong>{dateFmt}</strong> : null}
        <div className="grow flex flex-col">
          <p>{message}</p>
          {traceback &&
            <pre className="ml-4 text-sm italic lighten mt-1">{traceback}</pre>}
        </div>
        <div>
          <p className="text-xs text-end truncate self-end lighten">
            {partFile ? partFile.split("\\").pop() : "global"}
          </p>
          <p className="text-xs text-end truncate self-end lighten">
            {filename ?? ""}:{lineno ?? ""}
          </p>
        </div>
      </div>
    </>
  );
}
