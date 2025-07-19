# Screenshots Guide

This guide explains how to capture and add screenshots to your README.

## Required Screenshots

### 1. Dashboard Overview (dashboard-overview.png)
- **What to capture**: Full trading system window showing all three panels
- **When to capture**: After running `./TradingSystem` with live data
- **Size**: 1920x1080 or similar high resolution
- **Format**: PNG for best quality

### 2. Order Book Panel (order-book.png)
- **What to capture**: Close-up of the order book panel
- **Focus**: Bid/ask orders with prices and quantities visible
- **Size**: 600x400 recommended
- **Show**: Live orders with green (bids) and red (asks) colors

### 3. Portfolio Panel (portfolio.png)
- **What to capture**: Portfolio section showing position data
- **Focus**: Position size, P&L, risk status
- **Size**: 400x300 recommended
- **Show**: Active position with risk metrics

### 4. Trade History (trades.png)
- **What to capture**: Trade history panel with executed trades
- **Focus**: Recent executions with prices, quantities, order IDs
- **Size**: 600x400 recommended
- **Show**: Multiple completed trades

## How to Capture Screenshots

### On Linux (recommended for this project):
```bash
# Install screenshot tools
sudo apt install gnome-screenshot scrot

# Capture full screen
gnome-screenshot -f assets/screenshots/dashboard-overview.png

# Capture specific window
gnome-screenshot -w -f assets/screenshots/order-book.png

# Or use scrot for more control
scrot -s assets/screenshots/portfolio.png  # Select area with mouse
```

### On Windows:
- Use Snipping Tool or Snip & Sketch
- Windows + Shift + S for quick capture
- Save as PNG in assets/screenshots/

### On macOS:
- Cmd + Shift + 4 for area selection
- Cmd + Shift + 3 for full screen
- Save to assets/screenshots/

## Video Recording

### For Demo Video:
1. **Duration**: 30-60 seconds
2. **Content**: 
   - Start the system (`./TradingSystem`)
   - Show live order processing
   - Highlight GUI updates
   - Show trade executions
   - Display risk management in action

### Recording Tools:

**Linux:**
```bash
# Install OBS Studio or SimpleScreenRecorder
sudo apt install obs-studio
# or
sudo apt install simplescreenrecorder
```

**All Platforms:**
- OBS Studio (free, professional)
- Loom (online, easy to use)
- Asciinema (for terminal recordings)

### Video Formats:
- **MP4**: Best for GitHub
- **GIF**: For quick previews (< 10MB)
- **WebM**: Good compression

## Adding to README

Once you have the screenshots, the README_BEAUTIFUL.md already has placeholders:

```markdown
![Dashboard Overview](assets/screenshots/dashboard-overview.png)
![Order Book](assets/screenshots/order-book.png)
![Portfolio](assets/screenshots/portfolio.png)
![Trade History](assets/screenshots/trades.png)
```

## GitHub Video Embedding

For videos, upload to:
1. **GitHub Releases** (recommended)
2. **YouTube** (unlisted video)
3. **Direct upload to repo** (if < 25MB)

Then update the README:
```markdown
https://user-images.githubusercontent.com/your-username/video-id.mp4
```

## Quality Tips

- **High resolution**: At least 1200px width
- **Good lighting**: Make sure GUI is clearly visible
- **Clean background**: Close unnecessary windows
- **Show activity**: Capture during live trading simulation
- **Consistent style**: Use same window theme/size for all screenshots

Run the system first, then take screenshots of the live dashboard!
