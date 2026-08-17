import json
import urllib.request
from datetime import datetime, timedelta, timezone
from pathlib import Path

USERNAME = "Vedant_newcoder"

API_URL = (
    f"https://leetcode-api-pied.vercel.app/"
    f"user/{USERNAME}/calendar"
)

OUTPUT = Path("assets/leetcode-heatmap.svg")


def fetch_data():
    request = urllib.request.Request(
        API_URL,
        headers={"User-Agent": "Mozilla/5.0"}
    )

    with urllib.request.urlopen(request, timeout=30) as response:
        return json.loads(response.read().decode())


def main():
    data = fetch_data()

    # API normally returns the calendar directly.
    if "data" in data and isinstance(data["data"], dict):
        data = data["data"]

    calendar = data.get("submissionCalendar", {})

    if isinstance(calendar, str):
        calendar = json.loads(calendar)

    # Convert timestamp -> date -> submission count
    submissions = {}

    for timestamp, count in calendar.items():
        date = datetime.fromtimestamp(
            int(timestamp), tz=timezone.utc
        ).date()
        submissions[date] = int(count)

    today = datetime.now(timezone.utc).date()
    start = today - timedelta(days=364)

    # Keep only last 365 days
    submissions = {
        d: c for d, c in submissions.items()
        if start <= d <= today
    }

    total_submissions = sum(submissions.values())
    active_days = sum(1 for c in submissions.values() if c > 0)

    # Calculate maximum streak
    max_streak = 0
    current_streak = 0

    for i in range(365):
        day = start + timedelta(days=i)

        if submissions.get(day, 0) > 0:
            current_streak += 1
            max_streak = max(max_streak, current_streak)
        else:
            current_streak = 0

    # SVG dimensions
    width = 900
    height = 210

    cell = 12
    gap = 3

    # LeetCode-like dark background
    bg = "#282828"
    empty = "#3a3a3a"

    # Green intensity levels
    greens = [
        "#3a3a3a",
        "#0e4429",
        "#006d32",
        "#26a641",
        "#39d353",
    ]

    svg = []

    svg.append(
        f'<svg xmlns="http://www.w3.org/2000/svg" '
        f'width="{width}" height="{height}" '
        f'viewBox="0 0 {width} {height}">'
    )

    svg.append(
        f'<rect width="100%" height="100%" rx="10" fill="{bg}"/>'
    )

    # Header
    svg.append(
        '<text x="20" y="32" fill="#ffffff" '
        'font-family="Arial, sans-serif" font-size="20" '
        'font-weight="bold">'
        f'{total_submissions} submissions in the past one year'
        '</text>'
    )

    svg.append(
        '<text x="610" y="32" fill="#bdbdbd" '
        'font-family="Arial, sans-serif" font-size="12">'
        f'Total active days: {active_days}'
        '</text>'
    )

    svg.append(
        '<text x="780" y="32" fill="#bdbdbd" '
        'font-family="Arial, sans-serif" font-size="12">'
        f'Max streak: {max_streak}'
        '</text>'
    )

    # Find Sunday before start date
    calendar_start = start - timedelta(days=(start.weekday() + 1) % 7)

    # 53 weeks
    for week in range(53):

        x = 20 + week * (cell + gap)

        for day_index in range(7):

            day = calendar_start + timedelta(
                days=week * 7 + day_index
            )

            if day < start or day > today:
                continue

            count = submissions.get(day, 0)

            if count == 0:
                level = 0
            elif count == 1:
                level = 1
            elif count <= 3:
                level = 2
            elif count <= 5:
                level = 3
            else:
                level = 4

            y = 55 + day_index * (cell + gap)

            svg.append(
                f'<rect x="{x}" y="{y}" '
                f'width="{cell}" height="{cell}" rx="2" '
                f'fill="{greens[level]}">'
                f'<title>{day}: {count} submission(s)</title>'
                '</rect>'
            )

    # Month labels
    months_seen = set()

    for week in range(53):
        day = calendar_start + timedelta(days=week * 7)

        month_key = (day.year, day.month)

        if month_key in months_seen:
            continue

        months_seen.add(month_key)

        x = 20 + week * (cell + gap)

        svg.append(
            f'<text x="{x}" y="180" fill="#bdbdbd" '
            f'font-family="Arial, sans-serif" font-size="11">'
            f'{day.strftime("%b")}'
            '</text>'
        )

    # Legend
    svg.append(
        '<text x="20" y="202" fill="#888888" '
        'font-family="Arial, sans-serif" font-size="10">'
        'Less'
        '</text>'
    )

    for i in range(5):
        x = 48 + i * 16

        svg.append(
            f'<rect x="{x}" y="193" width="11" height="11" '
            f'rx="2" fill="{greens[i]}"/>'
        )

    svg.append(
        '<text x="135" y="202" fill="#888888" '
        'font-family="Arial, sans-serif" font-size="10">'
        'More'
        '</text>'
    )

    svg.append("</svg>")

    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT.write_text("\n".join(svg), encoding="utf-8")

    print(f"Generated {OUTPUT}")


if __name__ == "__main__":
    main()
