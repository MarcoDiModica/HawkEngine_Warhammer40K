using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Numerics;

namespace HawkEngine
{
    public class Pathfinding
    {
        private const int width = 5000;
        private const int height = 5000;
        private const float cellSize = 1f;
        private static readonly float DiagCost = (float)Math.Sqrt(2);

        public List<Vector3> FindPath(Vector3 startW, Vector3 endW)
        {
            (int sx, int sy) = ToGrid(startW);
            (int tx, int ty) = ToGrid(endW);

            if (!IsWalkable(sx, sy) || !IsWalkable(tx, ty))
                return null;

            var openSet = new List<Node> { new Node(sx, sy) { G = 0, H = Heuristic(sx, sy, tx, ty) } };
            var closedSet = new HashSet<Node>();

            while (openSet.Count > 0)
            {
                Node current = openSet.OrderBy(n => n.F).ThenBy(n => n.H).First();
                if (current.X == tx && current.Y == ty)
                    return Retrace(current);

                openSet.Remove(current);
                closedSet.Add(current);

                foreach (var nbr in GetNeighbors(current))
                {
                    if (closedSet.Contains(nbr)) continue;
                    if (!IsWalkable(nbr.X, nbr.Y)) continue;

                    float tentativeG = current.G + EdgeCost(current, nbr);
                    var existing = openSet.FirstOrDefault(n => n.Equals(nbr));
                    if (existing == null || tentativeG < existing.G)
                    {
                        nbr.G = tentativeG;
                        nbr.H = Heuristic(nbr.X, nbr.Y, tx, ty);
                        nbr.Parent = current;
                        if (existing == null)
                            openSet.Add(nbr);
                    }
                }
            }

            return null;
        }

        private static (int, int) ToGrid(Vector3 w)
        {
            int gx = (int)Math.Floor(w.X);
            int gy = (int)Math.Floor(w.Z);
            gx = Math.Max(0, Math.Min(width - 1, gx));
            gy = Math.Max(0, Math.Min(height - 1, gy));
            return (gx, gy);
        }

        private bool IsWalkable(int x, int y)
        {
            Vector3 center = new Vector3(x + .5f, .5f, y + .5f);
            float r = cellSize * .7f;
            var hits = Physics.OverlapSphere(center, r, "Obstacle");
                       //?? Array.Empty<GameObject>();
            return !hits.Any(go => go != null && go.tag == "Obstacle");
        }

        private IEnumerable<Node> GetNeighbors(Node n)
        {
            for (int dx = -1; dx <= 1; dx++)
                for (int dy = -1; dy <= 1; dy++)
                {
                    if (dx == 0 && dy == 0) continue;
                    int nx = n.X + dx, ny = n.Y + dy;
                    if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
                    if (dx != 0 && dy != 0)
                    {
                        if (!IsWalkable(n.X + dx, n.Y) || !IsWalkable(n.X, n.Y + dy))
                            continue;
                    }
                    yield return new Node(nx, ny);
                }
        }

        private static float EdgeCost(Node a, Node b) =>
            (a.X != b.X && a.Y != b.Y) ? DiagCost : 1f;

        private static float Heuristic(int x1, int y1, int x2, int y2)
        {
            return Math.Max(Math.Abs(x1 - x2), Math.Abs(y1 - y2));
        }

        private List<Vector3> Retrace(Node end)
        {
            var path = new List<Vector3>();
            for (Node curr = end; curr != null; curr = curr.Parent)
                path.Add(GridToWorld(curr));
            path.Reverse();
            return path;
        }

        private static Vector3 GridToWorld(Node n) =>
            new Vector3(n.X + .5f, .5f, n.Y + .5f);

        private class Node
        {
            public int X, Y;
            public float G = float.MaxValue, H;
            public Node Parent;
            public float F => G + H;
            public Node(int x, int y) { X = x; Y = y; }
            public override bool Equals(object o) =>
                o is Node n && n.X == X && n.Y == Y;
            public override int GetHashCode() => (X, Y).GetHashCode();
        }
    }
}
