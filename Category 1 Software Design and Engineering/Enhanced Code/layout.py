"""
layout.py - Defines the dashboard layout.
"""

from dash import html, dcc

def create_layout():
    """Return the main layout of the app."""
    return html.Div([
        html.H1("Animal Shelter Dashboard"),
        dcc.Input(id="search-box", type="text", placeholder="Search animals..."),
        dcc.Dropdown(
            id="filter-dropdown",
            options=[
                {"label": "Dog", "value": "Dog"},
                {"label": "Cat", "value": "Cat"},
                {"label": "Rabbit", "value": "Rabbit"}
            ],
            multi=True,
            placeholder="Filter by species"
        ),
        dcc.Graph(id="map"),
        html.Div(id="results-table")
    ])
