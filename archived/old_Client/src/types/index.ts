export type UseCountdownOptions = {
  onComplete?: () => void; // Called when countdown reaches 0
  onTick?: () => void; // Called every tick
};