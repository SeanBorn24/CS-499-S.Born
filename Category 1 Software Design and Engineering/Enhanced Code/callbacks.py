"""
callbacks.py - App interactions and data callbacks.
"""

from dash.dependencies import Input, Output
import pandas as pd

def register_callbacks(app, shelter):
    """Register all callbacks with the Dash app."""

    @app.callback(
        Output("results-table", "children"),
        Input("search-box", "value"),
        Input("filter-dropdown", "value")
    )
    def update_results(search_value, filters):
        """Update results table based on search and filters."""
        query = {}
        if search_value:
            query["name"] = {"$regex": search_value, "$options": "i"}
        if filters:
            query["animal_type"] = {"$in": filters}

        docs = shelter.read(query)
        if not docs:
            return "No results found."

        df = pd.DataFrame(docs)
        return df.to_html(classes="table table-striped")

    @app.callback(
        Output("map", "figure"),
        Input("filter-dropdown", "value")
    )
    def update_map(filters):
        """Update map to allow multiple animal locations at once."""
        query = {}
        if filters:
            query["animal_type"] = {"$in": filters}

        docs = shelter.read(query)
        if not docs:
            return {"data": [], "layout": {}}

        df = pd.DataFrame(docs)
        return {
            "data": [
                {
                    "lat": df["location_lat"],
                    "lon": df["location_lon"],
                    "text": df["name"],
                    "mode": "markers",
                    "type": "scattermapbox"
                }
            ],
            "layout": {
                "mapbox": {
                    "style": "open-street-map",
                    "center": {"lat": df["location_lat"].mean(), "lon": df["location_lon"].mean()},
                    "zoom": 10,
                },
                "margin": {"l": 0, "r": 0, "t": 0, "b": 0},
            }
        }

