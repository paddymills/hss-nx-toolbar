import type { Message } from "@/lib/types";
import { useMemo } from "react";
import { twMerge } from "tailwind-merge";

const TODAY = new Date(Date.now()).getDate();

const levelColor = (level: string) => {
  switch (level) {
    case "error":
      return "bg-red-100 text-red-800";
    case "warn":
      return "bg-yellow-100 text-yellow-800";
    case "debug":
      return "bg-green-100 text-green-800";
    case "info":
    default:
      return "bg-blue-100 text-blue-800";
  }
};

function LogLevelChip({ level, className }: { level: string, className?: string }) {
  const defaultStyles = "uppercase px-2 py-1 w-[8ch] rounded-md font-bold text-sm text-center";

  return (
    <span className={twMerge(levelColor(level), defaultStyles, className)}>{level}</span>
  );
}

interface LogItemProps extends Message {
  showDate?: boolean;
}

export function LogItem(props: LogItemProps) {
  const { timestamp, level, message, partFile, showDate = true } = props;

  const dateFmt = useMemo(() => {
    const date = new Date(timestamp);

    if (date.getDate() === TODAY) {
      return date.toLocaleTimeString();
    }

    return date.toLocaleString();
  }, [timestamp]);

  return (
    <div className="flex gap-2 mb-2 p-2 font-mono bordered">
      <LogLevelChip level={level} />
      {showDate ? <strong>{dateFmt}</strong> : null}
      <p className="grow">{message}</p>
      <p className="text-xs text-end truncate self-end lighten">{partFile ?? "global"}</p>
    </div>
  );
}
