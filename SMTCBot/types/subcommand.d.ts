import { CommandInteraction, SlashCommandSubcommandBuilder } from "discord.js";
import { ServerInfo } from "serverInfo";

export interface SubCommand {
    data: SlashCommandSubcommandBuilder;
    permissionLevel: number;
    init(): Promise<void> | void;
    execute(interaction: CommandInteraction, serverInfo: ServerInfo): Promise<void>;
}

declare const subCommand: SubCommand;
export default subCommand;