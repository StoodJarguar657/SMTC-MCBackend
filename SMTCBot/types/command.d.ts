import { ChatInputCommandInteraction, SlashCommandBuilder } from "discord.js";
import { ServerInfo } from "serverInfo";

export interface Command {
    data: SlashCommandBuilder;
    permissionLevel: number;
    init(): Promise<void> | void;
    execute(interaction: ChatInputCommandInteraction, serverInfo: ServerInfo, permissionLevel: number): Promise<void>;
}

declare const command: Command;
export default command;